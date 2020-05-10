
#include "scopetree.hpp"

#include <iostream>
#include <random>
#include "CppUTest/TestHarness.h"

TEST_GROUP(ScopeTreeTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ScopeTreeTests, createTree)
{
    NABLA::ScopeTree stree;

    /*                   /- d
                 /--- a --- e
                /
        root  ------- b  ---- f ---- i ---- j --- k --- l
                \         
                 \--- c  ---- g
                            \ h
    */



    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root", stree.createScope("a")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root", stree.createScope("b")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root", stree.createScope("c")));
 
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.a"   , stree.createScope("d")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.a"   , stree.createScope("e")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.c"   , stree.createScope("g")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.c"   , stree.createScope("h")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.b"   , stree.createScope("f")));

    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.b.f"   , stree.createScope("i")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.b.f.i"   , stree.createScope("j")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.b.f.i.j"   , stree.createScope("k")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.b.f.i.j.k"   , stree.createScope("l")));
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ScopeTreeTests, createFailures)
{
    NABLA::ScopeTree stree;

    /*
                 /----- b
        root --- a ---  c
                 \----- d ---- e
    */

    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root",   stree.createScope("a")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.a", stree.createScope("b")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.a", stree.createScope("c")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.a", stree.createScope("d")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.a.d", stree.createScope("e")));

    // These will be cleaned up by the scope tree despite the fact they are
    // rejectd by addChild below. NIFTY!
    NABLA::ScopeTree::Scope * t  = stree.createScope("b");
    NABLA::ScopeTree::Scope * t0 = stree.createScope("c");
    NABLA::ScopeTree::Scope * t1 = stree.createScope("d");
    NABLA::ScopeTree::Scope * t2 = stree.createScope("e");

    CHECK_TRUE(NABLA::ScopeTree::AddResult::DUPLICATE_CHILD_NAME == stree.addChild("root.a"  , t ));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::DUPLICATE_CHILD_NAME == stree.addChild("root.a"  , t0));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::DUPLICATE_CHILD_NAME == stree.addChild("root.a"  , t1));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::DUPLICATE_CHILD_NAME == stree.addChild("root.a.d", t2));
    
    CHECK_TRUE(NABLA::ScopeTree::AddResult::PARENT_NOT_FOUND == stree.addChild("root.b",     t));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::PARENT_NOT_FOUND == stree.addChild("root.a.e",   t0));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::PARENT_NOT_FOUND == stree.addChild("root.a.d.k", t1));

    CHECK_TRUE(NABLA::ScopeTree::AddResult::INVALID_DATA == stree.addChild("ayy",  nullptr));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::INVALID_DATA == stree.addChild("lmao", nullptr));
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ScopeTreeTests, actualTree)
{
    NABLA::ScopeTree stree;

    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root", stree.createScope("mod_a")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root", stree.createScope("mod_b")));

    
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.mod_a", stree.createScope("file_1")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.mod_a", stree.createScope("file_2")));

    
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.mod_b", stree.createScope("file_1")));
    CHECK_TRUE(NABLA::ScopeTree::AddResult::OKAY == stree.addChild("root.mod_b", stree.createScope("file_2")));
}
