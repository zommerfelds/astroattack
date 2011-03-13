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

TEST(LevelLoadTest, LoadWriteTest)
{
    SubSystems subSystems;
    Component::gameEvents = &subSystems.events;

    string lvlFileNames[] = { "data/Levels/level1.info",
                              "data/Levels/level2.info",
                              "data/Levels/level3.info",
                              "data/Levels/level_grav.info",
                              "data/Levels/level_visita.info",
                              "data/Levels/level_new.info",
                              "data/Levels/adjacent_problem.info" };
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
        EXPECT_TRUE(equal);
    }
}
