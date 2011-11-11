#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <list>

#include "common/DataLoader.h"
#include "common/World.h"
#include "game/GameEvents.h"
#include "game/GameApp.h"

using boost::property_tree::ptree;

using namespace std;

void dumpPropertyTree(const ptree& propertyTree, ostream& os, unsigned int identation=0)
{
    for (ptree::const_assoc_iterator it = propertyTree.ordered_begin(); it != propertyTree.not_found(); ++it)
    {
        std::string identationString (identation*3, ' ');

        os << identationString << it->first << " : " << it->second.data() << std::endl;
        dumpPropertyTree(it->second, os, identation+1);
    }
}

bool propTreeEquals(const ptree& propTree1, const ptree& propTree2)
{
    // map child names to all subtrees with that name (because two equal keys are possible)
    map<string, list<ptree> > children1;
    map<string, list<ptree> > children2;

    if (propTree1.size() != propTree2.size())
        return false;

    BOOST_FOREACH(const ptree::value_type &v, propTree1)
    {
        children1[v.first].push_back(v.second);
    }
    BOOST_FOREACH(const ptree::value_type &v, propTree2)
    {
        children2[v.first].push_back(v.second);
    }

    map<string, list<ptree> >::iterator it1 = children1.begin();
    map<string, list<ptree> >::iterator it2 = children2.begin();
    for (; it1 != children1.end(); ++it1, ++it2)
    {
        if (it1->first != it2->first) // compare node names
            return false;

        list<ptree>& l1 = it1->second;
        list<ptree>& l2 = it2->second;
        if (l1.size() != l2.size())
            return false;

        if (l1.size() == 1 && l1.front().data() != l2.front().data())
            return false;

        for (list<ptree>::iterator lit1 = l1.begin(); lit1 != l1.end();)
        {
            bool inc = true;
            for (list<ptree>::iterator lit2 = l2.begin(); lit2 != l2.end(); ++lit2)
            {
                if (propTreeEquals(*lit1, *lit2))
                {
                    // remove from list if a match is found
                    l2.erase(lit2);

                    list<ptree>::iterator next = lit1;
                    ++next;

                    l1.erase(lit1);
                    inc = false;
                    lit1 = next;
                    break;
                }
            }
            if (inc)
                ++lit1;
        }

        // if a tree was not removed (no match)
        if (!it1->second.empty())
            return false;
    }
    return true;
}

TEST(PropTreeEqualsTest, PropTreeEqualsTest)
{
    ptree p1;
    ptree p2;

    p1.add("bla", 1);
    p1.add("hoi", 2);
    p1.add("bli.ha", "m");
    p1.add("bli.ho", "m");
    p1.add("bli.hu", "m");

    p2.add("bli.hu", "m");
    p2.add("bli.ha", "m");
    p2.add("bli.ho", "m");
    p2.add("hoi", 2);
    p2.add("bla", 1);

    EXPECT_TRUE( propTreeEquals(p1, p2) );

    p1.add("mo.in.boo", 5);

    EXPECT_FALSE( propTreeEquals(p1, p2) );

    p2.add("mo.in.boo", "a");

    EXPECT_FALSE( propTreeEquals(p1, p2) );

    p2.add("mo.in.boo", 5);

    EXPECT_FALSE( propTreeEquals(p1, p2) );

    p1.add("mo.in.boo", "a");

    EXPECT_TRUE( propTreeEquals(p1, p2) );
}

TEST(LevelLoadTest, LoadWriteTest)
{
    SubSystems subSystems;

    string lvlFileNames[] = { "data/Levels/level1.info",
                              "data/Levels/level2.info",
                              "data/Levels/level3.info",
                              "data/Levels/level_grav.info",
                              "data/Levels/level_visita.info",
                              "data/Levels/level_new.info",
                              "data/Levels/adjacent_problem.info",
                              "data/player.info" };
    string lvlFileNameOut = "tmp_test_write_lvl.info";

    BOOST_FOREACH(string lvlFileName, lvlFileNames)
    {
        World world (subSystems.events);

        cout << "Testing with " << lvlFileName << endl;
        DataLoader::loadWorld(lvlFileName, world, subSystems);
        DataLoader::saveWorld(lvlFileNameOut, world);

        ptree levelPropTreeBefore;
        read_info(lvlFileName, levelPropTreeBefore);
        ptree levelPropTreeAfter;
        read_info(lvlFileNameOut, levelPropTreeAfter);

        bool equal = propTreeEquals(levelPropTreeBefore, levelPropTreeAfter);
        if (!equal)
        {
            cout << "--- before ---" << endl;
            dumpPropertyTree(levelPropTreeBefore, cout);
            cout << "--- after ---" << endl;
            dumpPropertyTree(levelPropTreeAfter, cout);
        }
        EXPECT_TRUE(equal);
    }
}
