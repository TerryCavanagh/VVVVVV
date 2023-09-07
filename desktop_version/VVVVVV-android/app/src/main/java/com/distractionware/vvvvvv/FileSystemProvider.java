/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.distractionware.vvvvvv;

import static android.provider.DocumentsContract.EXTRA_ORIENTATION;
import static android.provider.DocumentsContract.QUERY_ARG_DISPLAY_NAME;
import static android.provider.DocumentsContract.QUERY_ARG_EXCLUDE_MEDIA;
import static android.provider.DocumentsContract.QUERY_ARG_FILE_SIZE_OVER;
import static android.provider.DocumentsContract.QUERY_ARG_LAST_MODIFIED_AFTER;
import static android.provider.DocumentsContract.QUERY_ARG_MIME_TYPES;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.database.MatrixCursor.RowBuilder;
import android.graphics.Point;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.FileObserver;
import android.os.Handler;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;
import android.provider.DocumentsContract.Document;
import android.provider.DocumentsProvider;
import android.text.TextUtils;
import android.util.ArrayMap;
import android.util.Log;
import android.webkit.MimeTypeMap;

import androidx.core.content.MimeTypeFilter;
import androidx.exifinterface.media.ExifInterface;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.function.Predicate;

/**
 * A helper class for {@link android.provider.DocumentsProvider} to perform file operations on local
 * files.
 */
public abstract class FileSystemProvider extends DocumentsProvider {

    private static final String TAG = "FileSystemProvider";

    private static final boolean LOG_INOTIFY = false;

    protected static final String SUPPORTED_QUERY_ARGS = joinNewline(
        QUERY_ARG_DISPLAY_NAME,
        QUERY_ARG_FILE_SIZE_OVER,
        QUERY_ARG_LAST_MODIFIED_AFTER,
        QUERY_ARG_MIME_TYPES);

    private static String joinNewline(String... args) {
        return TextUtils.join("\n", args);
    }

    private String[] mDefaultProjection;

    private final ArrayMap<File, DirectoryObserver> mObservers = new ArrayMap<>();

    private Handler mHandler;

    protected abstract File getFileForDocId(String docId, boolean visible)
        throws FileNotFoundException;

    protected abstract String getDocIdForFile(File file) throws FileNotFoundException;

    protected abstract Uri buildNotificationUri(String docId);

    /**
     * Callback indicating that the given document has been modified. This gives the provider a hook
     * to invalidate cached data, such as {@code sdcardfs}.
     */
    protected void onDocIdChanged(String docId) {
        // Default is no-op
    }

    /**
     * Callback indicating that the given document has been deleted or moved. This gives the
     * provider a hook to revoke the uri permissions.
     */
    protected void onDocIdDeleted(String docId) {
        // Default is no-op
    }

    @Override
    public boolean onCreate() {
        throw new UnsupportedOperationException(
            "Subclass should override this and call onCreate(defaultDocumentProjection)");
    }

    protected void onCreate(String[] defaultProjection) {
        mHandler = new Handler();
        mDefaultProjection = defaultProjection;
    }

    private static boolean contains(String dirPath, String filePath) {
        if (dirPath.equals(filePath)) {
            return true;
        }
        if (!dirPath.endsWith("/")) {
            dirPath += "/";
        }
        return filePath.startsWith(dirPath);
    }

    private static boolean contains(File dir, File file) {
        if (dir == null || file == null) return false;
        return contains(dir.getAbsolutePath(), file.getAbsolutePath());
    }

    @Override
    public boolean isChildDocument(String parentDocId, String docId) {
        try {
            final File parent = getFileForDocId(parentDocId).getCanonicalFile();
            final File doc = getFileForDocId(docId).getCanonicalFile();
            return contains(parent, doc);
        } catch (IOException e) {
            throw new IllegalArgumentException(
                "Failed to determine if " + docId + " is child of " + parentDocId + ": " + e);
        }
    }

    protected final List<String> findDocumentPath(File parent, File doc)
        throws FileNotFoundException {

        if (!doc.exists()) {
            throw new FileNotFoundException(doc + " is not found.");
        }

        if (!contains(parent, doc)) {
            throw new FileNotFoundException(doc + " is not found under " + parent);
        }

        LinkedList<String> path = new LinkedList<>();
        while (doc != null && contains(parent, doc)) {
            path.addFirst(getDocIdForFile(doc));

            doc = doc.getParentFile();
        }

        return path;
    }

    private static boolean isValidFatFilenameChar(char c) {
        if ((0x00 <= c && c <= 0x1f)) {
            return false;
        }
        switch (c) {
            case '"':
            case '*':
            case '/':
            case ':':
            case '<':
            case '>':
            case '?':
            case '\\':
            case '|':
            case 0x7F:
                return false;
            default:
                return true;
        }
    }

    private static String buildValidFatFilename(String name) {
        if (TextUtils.isEmpty(name) || ".".equals(name) || "..".equals(name)) {
            return "(invalid)";
        }
        final StringBuilder res = new StringBuilder(name.length());
        for (int i = 0; i < name.length(); i++) {
            final char c = name.charAt(i);
            if (isValidFatFilenameChar(c)) {
                res.append(c);
            } else {
                res.append('_');
            }
        }
        // Even though vfat allows 255 UCS-2 chars, we might eventually write to
        // ext4 through a FUSE layer, so use that limit.
        trimFilename(res, 255);
        return res.toString();
    }

    private static String trimFilename(String str, int maxBytes) {
        final StringBuilder res = new StringBuilder(str);
        trimFilename(res, maxBytes);
        return res.toString();
    }

    private static void trimFilename(StringBuilder res, int maxBytes) {
        byte[] raw = res.toString().getBytes(StandardCharsets.UTF_8);
        if (raw.length > maxBytes) {
            maxBytes -= 3;
            while (raw.length > maxBytes) {
                res.deleteCharAt(res.length() / 2);
                raw = res.toString().getBytes(StandardCharsets.UTF_8);
            }
            res.insert(res.length() / 2, "...");
        }
    }

    private static File buildFile(File parent, String name, String ext) {
        if (TextUtils.isEmpty(ext)) {
            return new File(parent, name);
        } else {
            return new File(parent, name + "." + ext);
        }
    }

    private static File buildUniqueFileWithExtension(File parent, String name, String ext)
        throws FileNotFoundException {
        File file = buildFile(parent, name, ext);
        // If conflicting file, try adding counter suffix
        int n = 0;
        while (file.exists()) {
            if (n++ >= 32) {
                throw new FileNotFoundException("Failed to create unique file");
            }
            file = buildFile(parent, name + " (" + n + ")", ext);
        }
        return file;
    }

    private static String[] splitFileName(String mimeType, String displayName) {
        String name;
        String ext;
        if (Document.MIME_TYPE_DIR.equals(mimeType)) {
            name = displayName;
            ext = null;
        } else {
            String mimeTypeFromExt;
            // Extract requested extension from display name
            final int lastDot = displayName.lastIndexOf('.');
            if (lastDot >= 0) {
                name = displayName.substring(0, lastDot);
                ext = displayName.substring(lastDot + 1);
                mimeTypeFromExt = MimeTypeMap.getSingleton().getMimeTypeFromExtension(
                    ext.toLowerCase());
            } else {
                name = displayName;
                ext = null;
                mimeTypeFromExt = null;
            }
            if (mimeTypeFromExt == null) {
                mimeTypeFromExt = "application/octet-stream";
            }
            final String extFromMimeType;
            if ("application/octet-stream".equals(mimeType)) {
                extFromMimeType = null;
            } else {
                extFromMimeType = MimeTypeMap.getSingleton().getExtensionFromMimeType(mimeType);
            }
            if (Objects.equals(mimeType, mimeTypeFromExt) || Objects.equals(ext, extFromMimeType)) {
                // Extension maps back to requested MIME type; allow it
            } else {
                // No match; insist that create file matches requested MIME
                name = displayName;
                ext = extFromMimeType;
            }
        }
        if (ext == null) {
            ext = "";
        }
        return new String[]{name, ext};
    }

    private static File buildUniqueFile(File parent, String mimeType, String displayName)
        throws FileNotFoundException {
        final String[] parts = splitFileName(mimeType, displayName);
        return buildUniqueFileWithExtension(parent, parts[0], parts[1]);
    }

    @Override
    public String createDocument(String docId, String mimeType, String displayName)
        throws FileNotFoundException {
        displayName = buildValidFatFilename(displayName);

        final File parent = getFileForDocId(docId);
        if (!parent.isDirectory()) {
            throw new IllegalArgumentException("Parent document isn't a directory");
        }

        final File file = buildUniqueFile(parent, mimeType, displayName);
        final String childId;
        if (Document.MIME_TYPE_DIR.equals(mimeType)) {
            if (!file.mkdir()) {
                throw new IllegalStateException("Failed to mkdir " + file);
            }
            childId = getDocIdForFile(file);
            onDocIdChanged(childId);
        } else {
            try {
                if (!file.createNewFile()) {
                    throw new IllegalStateException("Failed to touch " + file);
                }
                childId = getDocIdForFile(file);
                onDocIdChanged(childId);
            } catch (IOException e) {
                throw new IllegalStateException("Failed to touch " + file + ": " + e);
            }
        }
        updateMediaStore(getContext(), file);
        return childId;
    }

    private static File buildUniqueFile(File parent, String displayName)
        throws FileNotFoundException {
        final String name;
        final String ext;
        // Extract requested extension from display name
        final int lastDot = displayName.lastIndexOf('.');
        if (lastDot >= 0) {
            name = displayName.substring(0, lastDot);
            ext = displayName.substring(lastDot + 1);
        } else {
            name = displayName;
            ext = null;
        }
        return buildUniqueFileWithExtension(parent, name, ext);
    }

    @Override
    public String renameDocument(String docId, String displayName) throws FileNotFoundException {
        // Since this provider treats renames as generating a completely new
        // docId, we're okay with letting the MIME type change.
        displayName = buildValidFatFilename(displayName);

        final File before = getFileForDocId(docId);
        final File beforeVisibleFile = getFileForDocId(docId, true);
        final File after = buildUniqueFile(before.getParentFile(), displayName);
        if (!before.renameTo(after)) {
            throw new IllegalStateException("Failed to rename to " + after);
        }

        final String afterDocId = getDocIdForFile(after);
        onDocIdChanged(docId);
        onDocIdDeleted(docId);
        onDocIdChanged(afterDocId);

        final File afterVisibleFile = getFileForDocId(afterDocId, true);

        updateMediaStore(getContext(), beforeVisibleFile);
        updateMediaStore(getContext(), afterVisibleFile);

        if (!TextUtils.equals(docId, afterDocId)) {
            return afterDocId;
        } else {
            return null;
        }
    }

    @Override
    public String moveDocument(String sourceDocumentId, String sourceParentDocumentId,
        String targetParentDocumentId)
        throws FileNotFoundException {
        final File before = getFileForDocId(sourceDocumentId);
        final File after = new File(getFileForDocId(targetParentDocumentId), before.getName());
        final File visibleFileBefore = getFileForDocId(sourceDocumentId, true);

        if (after.exists()) {
            throw new IllegalStateException("Already exists " + after);
        }
        if (!before.renameTo(after)) {
            throw new IllegalStateException("Failed to move to " + after);
        }

        final String docId = getDocIdForFile(after);
        onDocIdChanged(sourceDocumentId);
        onDocIdDeleted(sourceDocumentId);
        onDocIdChanged(docId);
        // update the database
        updateMediaStore(getContext(), visibleFileBefore);
        updateMediaStore(getContext(), getFileForDocId(docId, true));
        return docId;
    }

    private static void updateMediaStore(@NotNull Context context, File file) {
        if (file != null) {
            final ContentResolver resolver = context.getContentResolver();
            final String noMedia = ".nomedia";
            // For file, check whether the file name is .nomedia or not.
            // If yes, scan the parent directory to update all files in the directory.
            if (!file.isDirectory() && file.getName().toLowerCase(Locale.ROOT).endsWith(noMedia)) {
                MediaScannerConnection.scanFile(context, new String[]{file.getParent()}, null,
                                                null);
            } else {
                MediaScannerConnection.scanFile(context, new String[]{file.getParent()}, null,
                                                null);
            }
        }
    }

    private static boolean deleteContents(File dir) {
        File[] files = dir.listFiles();
        boolean success = true;
        if (files != null) {
            for (File file : files) {
                if (file.isDirectory()) {
                    success &= deleteContents(file);
                }
                if (!file.delete()) {
                    Log.w(TAG, "Failed to delete " + file);
                    success = false;
                }
            }
        }
        return success;
    }

    @Override
    public void deleteDocument(String docId) throws FileNotFoundException {
        final File file = getFileForDocId(docId);
        final File visibleFile = getFileForDocId(docId, true);

        final boolean isDirectory = file.isDirectory();
        if (isDirectory) {
            deleteContents(file);
        }
        // We could be deleting pending media which doesn't have any content yet, so only throw
        // if the file exists and we fail to delete it.
        if (file.exists() && !file.delete()) {
            throw new IllegalStateException("Failed to delete " + file);
        }

        onDocIdChanged(docId);
        onDocIdDeleted(docId);
        updateMediaStore(getContext(), visibleFile);
    }

    @Override
    public Cursor queryDocument(String documentId, String[] projection)
        throws FileNotFoundException {
        final MatrixCursor result = new MatrixCursor(resolveProjection(projection));
        includeFile(result, documentId, null);
        return result;
    }

    /**
     * This method is similar to
     * {@link DocumentsProvider#queryChildDocuments(String, String[], String)}. This method returns
     * all children documents including hidden directories/files.
     *
     * <p>
     * In a scoped storage world, access to "Android/data" style directories are hidden for privacy
     * reasons. This method may show privacy sensitive data, so its usage should only be in
     * restricted modes.
     *
     * @param parentDocumentId the directory to return children for.
     * @param projection       list of {@link Document} columns to put into the cursor. If
     *                         {@code null} all supported columns should be included.
     * @param sortOrder        how to order the rows, formatted as an SQL {@code ORDER BY} clause
     *                         (excluding the ORDER BY itself). Passing {@code null} will use the
     *                         default sort order, which may be unordered. This ordering is a hint
     *                         that can be used to prioritize how data is fetched from the network,
     *                         but UI may always enforce a specific ordering
     * @throws FileNotFoundException when parent document doesn't exist or query fails
     */
    protected Cursor queryChildDocumentsShowAll(
        String parentDocumentId, String[] projection, String sortOrder)
        throws FileNotFoundException {
        return queryChildDocuments(parentDocumentId, projection, sortOrder, File -> true);
    }

    @Override
    public Cursor queryChildDocuments(
        String parentDocumentId, String[] projection, String sortOrder)
        throws FileNotFoundException {
        // Access to some directories is hidden for privacy reasons.
        return queryChildDocuments(parentDocumentId, projection, sortOrder, this::shouldShow);
    }

    private static @NotNull File[] listFilesOrEmpty(@Nullable File dir) {
        if (dir == null) {
            return new File[]{};
        } else {
            File[] files = dir.listFiles();
            if (files == null) {
                return new File[]{};
            } else {
                return files;
            }
        }
    }

    private Cursor queryChildDocuments(
        String parentDocumentId, String[] projection, String sortOrder,
        @NotNull Predicate<File> filter) throws FileNotFoundException {
        final File parent = getFileForDocId(parentDocumentId);
        final MatrixCursor result = new DirectoryCursor(
            resolveProjection(projection), parentDocumentId, parent);

        if (!filter.test(parent)) {
            Log.w(TAG, "No permission to access parentDocumentId: " + parentDocumentId);
            return result;
        }

        if (parent.isDirectory()) {
            for (File file : listFilesOrEmpty(parent)) {
                if (filter.test(file)) {
                    includeFile(result, null, file);
                }
            }
        } else {
            Log.w(TAG, "parentDocumentId '" + parentDocumentId + "' is not Directory");
        }
        return result;
    }

    private static String[] getHandledQueryArguments(Bundle queryArgs) {
        if (queryArgs == null) {
            return new String[0];
        }
        final ArrayList<String> args = new ArrayList<>();
        if (queryArgs.keySet().contains(QUERY_ARG_EXCLUDE_MEDIA)) {
            args.add(QUERY_ARG_EXCLUDE_MEDIA);
        }
        if (queryArgs.keySet().contains(QUERY_ARG_DISPLAY_NAME)) {
            args.add(QUERY_ARG_DISPLAY_NAME);
        }
        if (queryArgs.keySet().contains(QUERY_ARG_FILE_SIZE_OVER)) {
            args.add(QUERY_ARG_FILE_SIZE_OVER);
        }
        if (queryArgs.keySet().contains(QUERY_ARG_LAST_MODIFIED_AFTER)) {
            args.add(QUERY_ARG_LAST_MODIFIED_AFTER);
        }
        if (queryArgs.keySet().contains(QUERY_ARG_MIME_TYPES)) {
            args.add(QUERY_ARG_MIME_TYPES);
        }
        return args.toArray(new String[0]);
    }

    /**
     * Searches documents under the given folder.
     * <p>
     * To avoid runtime explosion only returns the at most 23 items.
     *
     * @param folder     the root folder where recursive search begins
     * @param projection projection of the returned cursor
     * @param exclusion  absolute file paths to exclude from result
     * @param queryArgs  the query arguments for search
     * @return cursor containing search result. Include {@link ContentResolver#EXTRA_HONORED_ARGS}
     * in {@link Cursor} extras {@link Bundle} when any QUERY_ARG_* value was honored during the
     * preparation of the results.
     * @throws FileNotFoundException when root folder doesn't exist or search fails
     * @see ContentResolver#EXTRA_HONORED_ARGS
     */
    protected final Cursor querySearchDocuments(
        File folder, String[] projection, Set<String> exclusion, Bundle queryArgs)
        throws FileNotFoundException {
        final MatrixCursor result = new MatrixCursor(resolveProjection(projection));
        final LinkedList<File> pending = new LinkedList<>();
        pending.add(folder);
        while (!pending.isEmpty() && result.getCount() < 24) {
            final File file = pending.removeFirst();
            if (shouldHide(file)) continue;

            if (file.isDirectory()) {
                for (File child : listFilesOrEmpty(file)) {
                    pending.add(child);
                }
            }
            if (!exclusion.contains(file.getAbsolutePath()) && matchSearchQueryArguments(file,
                                                                                         queryArgs)) {
                includeFile(result, null, file);
            }
        }

        final String[] handledQueryArgs = getHandledQueryArguments(queryArgs);
        if (handledQueryArgs.length > 0) {
            final Bundle extras = new Bundle();
            extras.putStringArray(ContentResolver.EXTRA_HONORED_ARGS, handledQueryArgs);
            result.setExtras(extras);
        }
        return result;
    }

    @Override
    public String getDocumentType(String documentId) throws FileNotFoundException {
        return getDocumentType(documentId, getFileForDocId(documentId));
    }

    private String getDocumentType(final String documentId, final File file)
        throws FileNotFoundException {
        if (file.isDirectory()) {
            return Document.MIME_TYPE_DIR;
        } else {
            final int lastDot = documentId.lastIndexOf('.');
            if (lastDot >= 0) {
                final String extension = documentId.substring(lastDot + 1).toLowerCase();
                final String mime = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
                if (mime != null) {
                    return mime;
                }
            }
            return "application/octet-stream";
        }
    }

    @Override
    public ParcelFileDescriptor openDocument(
        String documentId, String mode, CancellationSignal signal)
        throws FileNotFoundException {
        final File file = getFileForDocId(documentId);
        final File visibleFile = getFileForDocId(documentId, true);

        final int pfdMode = ParcelFileDescriptor.parseMode(mode);
        if (visibleFile == null || pfdMode == ParcelFileDescriptor.MODE_READ_ONLY) {
            return ParcelFileDescriptor.open(file, pfdMode);
        } else {
            try {
                // When finished writing, kick off media scanner
                return ParcelFileDescriptor.open(
                    file, pfdMode, mHandler, (IOException e) -> {
                        onDocIdChanged(documentId);
                        scanFile(visibleFile);
                    });
            } catch (IOException e) {
                throw new FileNotFoundException("Failed to open for writing: " + e);
            }
        }
    }

    public static boolean matchSearchQueryArguments(Bundle queryArgs, String displayName,
        String mimeType, long lastModified, long size) {
        if (queryArgs == null) {
            return true;
        }
        final String argDisplayName = queryArgs.getString(QUERY_ARG_DISPLAY_NAME, "");
        if (!argDisplayName.isEmpty()) {
            // TODO (118795812) : Enhance the search string handled in DocumentsProvider
            if (!displayName.toLowerCase().contains(argDisplayName.toLowerCase())) {
                return false;
            }
        }
        final long argFileSize = queryArgs.getLong(QUERY_ARG_FILE_SIZE_OVER, -1 /* defaultValue */);
        if (argFileSize != -1 && size < argFileSize) {
            return false;
        }
        final long argLastModified = queryArgs.getLong(QUERY_ARG_LAST_MODIFIED_AFTER,
                                                       -1 /* defaultValue */);
        if (argLastModified != -1 && lastModified < argLastModified) {
            return false;
        }
        final String[] argMimeTypes = queryArgs.getStringArray(QUERY_ARG_MIME_TYPES);
        if (argMimeTypes != null && argMimeTypes.length > 0) {
            mimeType = Intent.normalizeMimeType(mimeType);
            for (String type : argMimeTypes) {
                if (MimeTypeFilter.matches(mimeType, Intent.normalizeMimeType(type))) {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    /**
     * Test if the file matches the query arguments.
     *
     * @param file      the file to test
     * @param queryArgs the query arguments
     */
    private boolean matchSearchQueryArguments(File file, Bundle queryArgs) {
        if (file == null) {
            return false;
        }

        final String fileMimeType;
        final String fileName = file.getName();

        if (file.isDirectory()) {
            fileMimeType = DocumentsContract.Document.MIME_TYPE_DIR;
        } else {
            int dotPos = fileName.lastIndexOf('.');
            if (dotPos < 0) {
                return false;
            }
            final String extension = fileName.substring(dotPos + 1);
            fileMimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
        }
        return matchSearchQueryArguments(queryArgs, fileName, fileMimeType,
                                                           file.lastModified(), file.length());
    }

    private void scanFile(File visibleFile) {
        final Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        intent.setData(Uri.fromFile(visibleFile));
        getContext().sendBroadcast(intent);
    }

    private static AssetFileDescriptor openImageThumbnail(File file) throws FileNotFoundException {
        final ParcelFileDescriptor pfd = ParcelFileDescriptor.open(
            file, ParcelFileDescriptor.MODE_READ_ONLY);
        try {
            final ExifInterface exif = new ExifInterface(file.getAbsolutePath());
            final long[] thumb = exif.getThumbnailRange();
            if (thumb != null) {
                // If we use thumb to decode, we need to handle the rotation by ourselves.
                Bundle extras = null;
                switch (exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, -1)) {
                    case ExifInterface.ORIENTATION_ROTATE_90:
                        extras = new Bundle(1);
                        extras.putInt(EXTRA_ORIENTATION, 90);
                        break;
                    case ExifInterface.ORIENTATION_ROTATE_180:
                        extras = new Bundle(1);
                        extras.putInt(EXTRA_ORIENTATION, 180);
                        break;
                    case ExifInterface.ORIENTATION_ROTATE_270:
                        extras = new Bundle(1);
                        extras.putInt(EXTRA_ORIENTATION, 270);
                        break;
                }
                return new AssetFileDescriptor(pfd, thumb[0], thumb[1], extras);
            }
        } catch (IOException ignored) {
        }
        // Do full file decoding, we don't need to handle the orientation
        return new AssetFileDescriptor(pfd, 0, AssetFileDescriptor.UNKNOWN_LENGTH, null);
    }

    @Override
    public AssetFileDescriptor openDocumentThumbnail(
        String documentId, Point sizeHint, CancellationSignal signal)
        throws FileNotFoundException {
        final File file = getFileForDocId(documentId);
        return openImageThumbnail(file);
    }

    protected RowBuilder includeFile(final MatrixCursor result, String docId, File file)
        throws FileNotFoundException {
        final String[] columns = result.getColumnNames();
        final RowBuilder row = result.newRow();

        if (docId == null) {
            docId = getDocIdForFile(file);
        } else {
            file = getFileForDocId(docId);
        }

        final String mimeType = getDocumentType(docId, file);
        row.add(Document.COLUMN_DOCUMENT_ID, docId);
        row.add(Document.COLUMN_MIME_TYPE, mimeType);

        final int flagIndex = Arrays.asList(columns).indexOf(Document.COLUMN_FLAGS);
        if (flagIndex != -1) {
            int flags = 0;
            if (file.canWrite()) {
                if (mimeType.equals(Document.MIME_TYPE_DIR)) {
                    flags |= Document.FLAG_DIR_SUPPORTS_CREATE;
                    flags |= Document.FLAG_SUPPORTS_DELETE;
                    flags |= Document.FLAG_SUPPORTS_RENAME;
                    flags |= Document.FLAG_SUPPORTS_MOVE;
                } else {
                    flags |= Document.FLAG_SUPPORTS_WRITE;
                    flags |= Document.FLAG_SUPPORTS_DELETE;
                    flags |= Document.FLAG_SUPPORTS_RENAME;
                    flags |= Document.FLAG_SUPPORTS_MOVE;
                }
            }

            if (mimeType.startsWith("image/")) {
                flags |= Document.FLAG_SUPPORTS_THUMBNAIL;
            }

            if (typeSupportsMetadata(mimeType)) {
                flags |= Document.FLAG_SUPPORTS_METADATA;
            }
            row.add(Document.COLUMN_FLAGS, flags);
        }

        final int displayNameIndex = Arrays.asList(columns).indexOf(Document.COLUMN_DISPLAY_NAME);
        if (displayNameIndex != -1) {
            row.add(Document.COLUMN_DISPLAY_NAME, file.getName());
        }

        final int lastModifiedIndex = Arrays.asList(columns).indexOf(Document.COLUMN_LAST_MODIFIED);
        if (lastModifiedIndex != -1) {
            final long lastModified = file.lastModified();
            // Only publish dates reasonably after epoch
            if (lastModified > 31536000000L) {
                row.add(Document.COLUMN_LAST_MODIFIED, lastModified);
            }
        }
        final int sizeIndex = Arrays.asList(columns).indexOf(Document.COLUMN_SIZE);
        if (sizeIndex != -1) {
            row.add(Document.COLUMN_SIZE, file.length());
        }

        // Return the row builder just in case any subclass want to add more stuff to it.
        return row;
    }

    protected boolean shouldHide(@NotNull File file) {
        return false;
    }

    private boolean shouldShow(@NotNull File file) {
        return !shouldHide(file);
    }

    protected boolean typeSupportsMetadata(String mimeType) {
        return Document.MIME_TYPE_DIR.equals(mimeType);
    }

    protected final File getFileForDocId(String docId) throws FileNotFoundException {
        return getFileForDocId(docId, false);
    }

    private String[] resolveProjection(String[] projection) {
        return projection == null ? mDefaultProjection : projection;
    }

    private void startObserving(File file, Uri notifyUri, DirectoryCursor cursor) {
        synchronized (mObservers) {
            DirectoryObserver observer = mObservers.get(file);
            if (observer == null) {
                observer =
                    new DirectoryObserver(file, getContext().getContentResolver(), notifyUri);
                observer.startWatching();
                mObservers.put(file, observer);
            }
            observer.mCursors.add(cursor);

            if (LOG_INOTIFY) Log.d(TAG, "after start: " + observer);
        }
    }

    private void stopObserving(File file, DirectoryCursor cursor) {
        synchronized (mObservers) {
            DirectoryObserver observer = mObservers.get(file);
            if (observer == null) return;

            observer.mCursors.remove(cursor);
            if (observer.mCursors.size() == 0) {
                mObservers.remove(file);
                observer.stopWatching();
            }

            if (LOG_INOTIFY) Log.d(TAG, "after stop: " + observer);
        }
    }

    private static class DirectoryObserver extends FileObserver {
        private static final int NOTIFY_EVENTS = ATTRIB | CLOSE_WRITE | MOVED_FROM | MOVED_TO
            | CREATE | DELETE | DELETE_SELF | MOVE_SELF;

        private final File mFile;
        private final ContentResolver mResolver;
        private final Uri mNotifyUri;
        private final CopyOnWriteArrayList<DirectoryCursor> mCursors;

        DirectoryObserver(File file, ContentResolver resolver, Uri notifyUri) {
            super(file.getAbsolutePath(), NOTIFY_EVENTS);
            mFile = file;
            mResolver = resolver;
            mNotifyUri = notifyUri;
            mCursors = new CopyOnWriteArrayList<>();
        }

        @Override
        public void onEvent(int event, String path) {
            if ((event & NOTIFY_EVENTS) != 0) {
                if (LOG_INOTIFY) Log.d(TAG, "onEvent() " + event + " at " + path);
                for (DirectoryCursor cursor : mCursors) {
                    cursor.notifyChanged();
                }
                mResolver.notifyChange(mNotifyUri, null, false);
            }
        }

        @Override
        public String toString() {
            String filePath = mFile.getAbsolutePath();
            return "DirectoryObserver{file=" + filePath + ", ref=" + mCursors.size() + "}";
        }
    }

    private class DirectoryCursor extends MatrixCursor {
        private final File mFile;

        public DirectoryCursor(String[] columnNames, String docId, File file) {
            super(columnNames);

            final Uri notifyUri = buildNotificationUri(docId);
            boolean registerSelfObserver = false; // Our FileObserver sees all relevant changes.
            setNotificationUris(getContext().getContentResolver(), Arrays.asList(notifyUri));

            mFile = file;
            startObserving(mFile, notifyUri, this);
        }

        public void notifyChanged() {
            onChange(false);
        }

        @Override
        public void close() {
            super.close();
            stopObserving(mFile, this);
        }
    }
}
