// Forward decl, avoid including tinyxml2.h
namespace tinyxml2
{
    class XMLComment;
    class XMLDocument;
    class XMLDeclaration;
    class XMLElement;
    class XMLNode;
}

namespace xml
{

tinyxml2::XMLDocument& get_document(tinyxml2::XMLNode* parent);

tinyxml2::XMLElement* update_element(tinyxml2::XMLNode* parent, const char* name);
// Same thing as above, but takes &parent instead of *parent
tinyxml2::XMLElement* update_element(tinyxml2::XMLNode& parent, const char* name);

tinyxml2::XMLElement* update_element_delete_contents(tinyxml2::XMLNode* parent, const char* name);

tinyxml2::XMLElement* update_tag(tinyxml2::XMLNode* parent, const char* name, const char* value);
tinyxml2::XMLElement* update_tag(tinyxml2::XMLNode* parent, const char* name, const int value);

tinyxml2::XMLDeclaration* update_declaration(tinyxml2::XMLDocument& doc);

tinyxml2::XMLComment* update_comment(tinyxml2::XMLNode* parent, const char* text);

} // namespace xml
