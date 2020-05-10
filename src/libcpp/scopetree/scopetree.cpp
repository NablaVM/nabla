#include "scopetree.hpp"

#include <iostream>
#include <sstream>

namespace NABLA
{
    // ------------------------------------------------------
    //
    // ------------------------------------------------------

    struct ScopeTree::Scope
    {
        std::string name;
        Scope * parent;
        std::vector<Scope*> children;
    };

    // ------------------------------------------------------
    //
    // ------------------------------------------------------

    ScopeTree::ScopeTree()
    {
        // Create the root scope object
        scopeObjectsInUse.push_back( createScope("root") );
    }

    // ------------------------------------------------------
    //
    // ------------------------------------------------------

    ScopeTree::~ScopeTree()
    {
        for(auto & i : allCreatedScopeObjects)
        {
            // Incase someone manually deletes a scope that is in the list 
            if(i != nullptr)
            {
                delete i;
            }
        }
    }

    // ------------------------------------------------------
    //
    // ------------------------------------------------------

    ScopeTree::Scope * ScopeTree::createScope(std::string name)
    {
        Scope * ns = new Scope;
        ns->name = name;
        ns->parent = nullptr;

        allCreatedScopeObjects.push_back(ns);
        return ns;
    }

    // ------------------------------------------------------
    //
    // ------------------------------------------------------

    ScopeTree::AddResult ScopeTree::addChild(std::string parentPath, ScopeTree::Scope* child)
    {
        // Stop soemthing dumb
        if(child == nullptr){ return AddResult::INVALID_DATA; }

        // We need to split the path that should be of the form  : "root.path.name.every.spot.is.a.child"
        std::string token;
        std::istringstream ss(parentPath);

        // Get the root
        Scope * parentScope = scopeObjectsInUse[0];

        bool rootFlag = true;

        // Locate the parent
        while(std::getline(ss, token, '.')) 
        {
            // Edge case right out the gate - Skip the first node, 
            // but ensure if something else has the name 'root' we don't ignore it
            if(rootFlag && token == "root")
            {
                rootFlag = false;
                continue;
            }

            bool tokenMatched = false;

            // Check if current token is in the parentScope children
            for(auto & c : parentScope->children)
            {
                // Ensure they didn't try to delete the thing externally
                if(c == nullptr){ return AddResult::INVALID_DATA; }

                // If the child has the name specified, we need to drill further in
                if(c->name == token)
                {
                    tokenMatched = true;
                    parentScope = c;
                }
            }

            // If the token wasn't matched in the child list then we cant find the parent
            // so we need to error out
            if(!tokenMatched)
            {
                return AddResult::PARENT_NOT_FOUND;
            }
        }

        // The target parent should no be "parentScope"

        // Check if the requested child to add is duplicate
        for(auto & c : parentScope->children)
        {
            if(c->name == child->name)
            {
                return AddResult::DUPLICATE_CHILD_NAME;
            }
        }

        // Add the child
        parentScope->children.push_back(child);

        return AddResult::OKAY;
    }

    // ------------------------------------------------------
    //
    // ------------------------------------------------------

}

/*


        if(child == nullptr){ return AddResult::INVALID_DATA; }

        // Add to scope tree
        for(auto & i : scopeObjectsInUse)
        {
            // If someone deleted what i points to externally they did a bad 
            if(i == nullptr) { return AddResult::INVALID_DATA; }

            // If the parent is found
            if(i->name == parent)
            {
                for(auto & c : i->children)
                {
                    // Children must be unique
                    if(c->name == child->name)
                    {
                        return AddResult::DUPLICATE_CHILD_NAME;
                    }
                }

                // Set the child's parent
                child->parent = i;
                i->children.push_back(child);
                
                // Add child to known scope objects
                scopeObjectsInUse.push_back( child );
                return AddResult::OKAY;
            }
        }
        return AddResult::PARENT_NOT_FOUND;
*/