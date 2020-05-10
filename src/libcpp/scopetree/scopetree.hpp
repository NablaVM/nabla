/*

    This is a simple tree maker for handling scope resolution in Nabla the HLL.

    It creates a series of connected Scope objects that helps dictate if something
    is 'in scope' 

    The tree doesn't care if cycles are present. If a cycle is made, it is made, 
    and won't break anything
*/

#ifndef NABLA_SCOPE_TREE_HPP
#define NABLA_SCOPE_TREE_HPP

#include <string>
#include <vector>

namespace NABLA
{

    //! \brief A tree that defines scope for NHLL
    class ScopeTree
    {
    public:

        enum class AddResult
        {
            OKAY,
            PARENT_NOT_FOUND,
            DUPLICATE_CHILD_NAME,
            INVALID_DATA
        };

        //! \brief The scope structure (defined in source)
        //! \note  The scope implementation is hidden, so ScopeTree::createScope()
        //!        must be used to create the object, and the scope tree needs to
        //!        be used to edit the scope
        struct Scope;

        //! \brief Create a scope tree
        //! \post  A scope with the name 'root' will be created
        ScopeTree();

        //! \brief Destruct the tree. Deletes all scope objects created by createScope
        ~ScopeTree();

        //! \brief Creates a scope and returns pointer to it
        //! \param name The name of the scope
        //! \returns Pointer to scope object 
        //! \note    Object wont be deleted automatically unless added as child
        Scope * createScope(std::string name);

        //! \brief Add a child to a named parent scope node
        //! \param parentPath Parent path to item  ( root.moda.file1.val )
        //! \param child The pointer to a child scope object
        //! \returns AddResult enum for result of operation
        AddResult addChild(std::string parentPath, Scope * child);

        //! \brief Check if item is in a scope
        //! \param scopePath The path of the scope
        //! \param item The item to check is in scope
        //! \returns true if in scope, false if scope path is invalid, or 
        //!          item not in scope
        bool isItemInScope(std::string scopePath, std::string item);

    private:

        // The scope objects that are being used in the tree structure
        // This should be the same as allCreatedObjects, but that cant 
        // be made into a promise. Oh well
        std::vector<Scope*> scopeObjectsInUse;

        // All created scope objects for easy clean up
        std::vector<Scope*> allCreatedScopeObjects;
    };
}


#endif