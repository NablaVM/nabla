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

    bool ScopeTree::isItemInScope(std::string path, std::string item)
    {
         // We need to split the path that should be of the form  : "root.path.name.every.spot.is.a.child"
        std::string token;
        std::istringstream ss(path);

        // Get the root
        Scope * scope = scopeObjectsInUse[0];

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

            // Check if current token is in the scope children
            for(auto & c : scope->children)
            {
                // Ensure they didn't try to delete the thing externally
                if(c == nullptr){ return false; }

                // If the child has the name specified, we need to drill further in
                if(c->name == token)
                {
                    tokenMatched = true;
                    scope = c;
                }
            }

            // If the token wasn't matched in the child list then we cant find the parent
            // so we need to error out
            if(!tokenMatched)
            {
                return false;
            }
        }

        // Check if the item is a child of the scope
        for(auto & i : scope->children)
        {
            if(i->name == item)
            {
                return true;
            }
        }

        return false;
    }
}