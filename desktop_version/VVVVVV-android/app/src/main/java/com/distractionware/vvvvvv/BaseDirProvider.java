package com.distractionware.vvvvvv;

import android.database.Cursor;
import android.database.MatrixCursor;
import android.database.MatrixCursor.RowBuilder;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.DocumentsContract.Document;
import android.provider.DocumentsContract.Root;

import java.io.File;
import java.io.FileNotFoundException;

public class BaseDirProvider extends FileSystemProvider {
    private static final String AUTHORITY = "com.distractionware.vvvvvv.basedir";
    private static final String DOC_ID_ROOT = "vvvvvv";

    private static final String[] DEFAULT_ROOT_PROJECTION = new String[]{
        Root.COLUMN_ROOT_ID, Root.COLUMN_FLAGS, Root.COLUMN_ICON, Root.COLUMN_TITLE,
        Root.COLUMN_DOCUMENT_ID,
    };

    private static final String[] DEFAULT_DOCUMENT_PROJECTION = new String[]{
        Document.COLUMN_DOCUMENT_ID, Document.COLUMN_MIME_TYPE, Document.COLUMN_DISPLAY_NAME,
        Document.COLUMN_LAST_MODIFIED, Document.COLUMN_FLAGS, Document.COLUMN_SIZE,
    };

    private File mRoot;

    @Override
    public boolean onCreate() {
        super.onCreate(DEFAULT_DOCUMENT_PROJECTION);
        mRoot = getContext().getExternalFilesDir(null);
        return true;
    }

    @Override
    public Cursor queryRoots(String[] projection) throws FileNotFoundException {
        final MatrixCursor result = new MatrixCursor(resolveRootProjection(projection));
        final RowBuilder row = result.newRow();
        row.add(Root.COLUMN_ROOT_ID, DOC_ID_ROOT);
        row.add(Root.COLUMN_FLAGS,
                Root.FLAG_LOCAL_ONLY | Root.FLAG_SUPPORTS_CREATE | Root.FLAG_SUPPORTS_SEARCH |
                    Root.FLAG_SUPPORTS_IS_CHILD);
        row.add(Root.COLUMN_ICON, R.mipmap.ic_launcher);
        row.add(Root.COLUMN_TITLE, "VVVVVV");
        row.add(Root.COLUMN_DOCUMENT_ID, DOC_ID_ROOT);
        return result;
    }

    @Override
    protected Uri buildNotificationUri(String docId) {
        return DocumentsContract.buildChildDocumentsUri(AUTHORITY, docId);
    }

    private static String[] resolveRootProjection(String[] projection) {
        return projection != null ? projection : DEFAULT_ROOT_PROJECTION;
    }

    private static String[] resolveDocumentProjection(String[] projection) {
        return projection != null ? projection : DEFAULT_DOCUMENT_PROJECTION;
    }

    @Override
    protected String getDocIdForFile(File file) {
        return "raw:" + file.getAbsolutePath();
    }

    @Override
    protected File getFileForDocId(String documentId, boolean visible)
        throws FileNotFoundException {
        if (DOC_ID_ROOT.equals(documentId)) {
            return mRoot;
        } else if (documentId.startsWith("raw:")) {
            final String name = documentId.substring(4);
            final File file = new File(name);
            if (!file.exists()) {
                throw new FileNotFoundException("File not found: " + name);
            }
            return file;
        } else {
            throw new FileNotFoundException("Invalid document ID: " + documentId);
        }
    }
}
