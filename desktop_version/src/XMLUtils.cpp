#include <SDL.h>
#include <tinyxml2.h>

namespace xml
{

// Just get the document, because TinyXML-2 is annoying.
// Useful outside of this file.
// TODO: But XML handling should really be put in a separate file (maybe this
// one renamed?) instead of lumped in with Game.cpp, and when that happens
// maybe this will be unexported again?
tinyxml2::XMLDocument& get_document(tinyxml2::XMLNode* parent)
{
    return *(parent->GetDocument());
}

// Create a new element if it doesn't exist. Returns the element.
tinyxml2::XMLElement* update_element(tinyxml2::XMLNode* parent, const char* name)
{
    if (parent == NULL)
    {
        return NULL;
    }

    tinyxml2::XMLDocument& doc = get_document(parent);

    tinyxml2::XMLElement* element = parent->FirstChildElement(name);
    if (element == NULL)
    {
        // It doesn't exist, so create a new one
        element = doc.NewElement(name);
        parent->LinkEndChild(element);
    }

    return element;
}

// Same thing as above, but takes &parent instead of *parent
tinyxml2::XMLElement* update_element(tinyxml2::XMLNode& parent, const char* name)
{
    return update_element(&parent, name);
}

// Same thing as above, but deletes the content inside the element, too.
tinyxml2::XMLElement* update_element_delete_contents(tinyxml2::XMLNode* parent, const char* name)
{
    tinyxml2::XMLDocument& doc = get_document(parent);

    tinyxml2::XMLElement* element = update_element(parent, name);

    for (tinyxml2::XMLNode* node = element->FirstChild();
    node != NULL;
    /* Increment code handled separately */)
    {
        // Unfortunately, element->DeleteNode() is private
        // Can't just doc.DeleteNode(node) and then go to next,
        // node->NextSiblingElement() will be NULL.
        // Instead, store pointer of node we want to delete. Then increment
        // `node`. And THEN delete the node.
        tinyxml2::XMLNode* delete_this = node;

        node = node->NextSiblingElement();

        doc.DeleteNode(delete_this);
    }

    return element;
}

// Call update_element_delete_contents(), then immediately set its value to a
// string. Returns the element.
tinyxml2::XMLElement* update_tag(tinyxml2::XMLNode* parent, const char* name, const char* value)
{
    tinyxml2::XMLElement* element = update_element_delete_contents(parent, name);

    element->InsertNewText(value);

    return element;
}

// Same as above, but takes an int instead, and automatically converts it to a
// string.
tinyxml2::XMLElement* update_tag(tinyxml2::XMLNode* parent, const char* name, const int value)
{
    char string[16];
    SDL_snprintf(string, sizeof(string), "%i", value);

    return update_tag(parent, name, string);
}

// Delete the declaration, if it exists. Then create a new declaration.
// Returns the declaration.
tinyxml2::XMLDeclaration* update_declaration(tinyxml2::XMLDocument& doc)
{
    if (doc.FirstChild() != NULL)
    {
        tinyxml2::XMLDeclaration* decl = doc.FirstChild()->ToDeclaration();

        if (decl != NULL)
        {
            doc.DeleteNode(decl);
        }
    }

    tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
    doc.InsertFirstChild(decl);

    return decl;
}

// Create a new comment if the first child of the node isn't a comment.
// Returns the comment.
tinyxml2::XMLComment* update_comment(tinyxml2::XMLNode* parent, const char* text)
{
    if (parent == NULL)
    {
        return NULL;
    }

    tinyxml2::XMLDocument& doc = get_document(parent);

    if (parent->FirstChild() == NULL)
    {
        return NULL;
    }

    tinyxml2::XMLComment* comment = parent->FirstChild()->ToComment();

    if (comment == NULL)
    {
        // It doesn't exist, so create a new one
        comment = doc.NewComment(text);
        parent->InsertFirstChild(comment);
    }

    return comment;
}



} // namespace xml
