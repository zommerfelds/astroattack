#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>
#include <string>

#include "DataLoader.h"
#include "World.h"
#include "GameEvents.h"
#include "GameApp.h"

using boost::property_tree::ptree;

using namespace std;

void dumpPropertyTree(const ptree& propertyTree, ostream& os, unsigned int identation=0)
{
    BOOST_FOREACH(const ptree::value_type &v, propertyTree)
    {
        std::string identationString (identation*3, ' ');

        os << identationString << v.first << " : " << v.second.data() << std::endl;
        dumpPropertyTree(v.second, os, identation+1);
    }
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
        GameWorld world (subSystems.events);

        DataLoader::loadWorld(lvlFileName, world, subSystems);
        DataLoader::saveWorldToXml(lvlFileNameOut, world);

        ptree levelPropTreeBefore;
        read_info(lvlFileName, levelPropTreeBefore);
        ptree levelPropTreeAfter;
        read_info(lvlFileNameOut, levelPropTreeAfter);

        bool equal = (levelPropTreeBefore == levelPropTreeAfter);
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
