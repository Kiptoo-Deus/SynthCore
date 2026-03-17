#include <gtest/gtest.h>
#include "synthcore/SampleMap.hpp"

TEST(SampleMapTest, AddAndCountRegions) {
    synthcore::SampleMap map;
    
    synthcore::SampleRegion region1;
    region1.rootNote = 60;
    region1.loNote = 60;
    region1.hiNote = 60;
    
    map.addRegion(region1);
    EXPECT_EQ(map.getRegionCount(), 1);
    
    synthcore::SampleRegion region2;
    region2.rootNote = 61;
    region2.loNote = 61;
    region2.hiNote = 61;
    
    map.addRegion(region2);
    EXPECT_EQ(map.getRegionCount(), 2);
}

TEST(SampleMapTest, VelocityLayerLookup) {
    synthcore::SampleMap map;
    
    synthcore::SampleRegion ppRegion;
    ppRegion.rootNote = 60;
    ppRegion.loNote = 60;
    ppRegion.hiNote = 60;
    ppRegion.loVel = 0;
    ppRegion.hiVel = 42;
    ppRegion.samples = {0.1f};
    
    synthcore::SampleRegion mfRegion;
    mfRegion.rootNote = 60;
    mfRegion.loNote = 60;
    mfRegion.hiNote = 60;
    mfRegion.loVel = 43;
    mfRegion.hiVel = 84;
    mfRegion.samples = {0.5f};
    
    synthcore::SampleRegion ffRegion;
    ffRegion.rootNote = 60;
    ffRegion.loNote = 60;
    ffRegion.hiNote = 60;
    ffRegion.loVel = 85;
    ffRegion.hiVel = 127;
    ffRegion.samples = {1.0f};
    
    map.addRegion(ppRegion);
    map.addRegion(mfRegion);
    map.addRegion(ffRegion);
    
    const auto* result20 = map.lookup(60, 20);
    ASSERT_NE(result20, nullptr);
    EXPECT_FLOAT_EQ(result20->samples[0], 0.1f);
    
    const auto* result64 = map.lookup(60, 64);
    ASSERT_NE(result64, nullptr);
    EXPECT_FLOAT_EQ(result64->samples[0], 0.5f);
    
    const auto* result100 = map.lookup(60, 100);
    ASSERT_NE(result100, nullptr);
    EXPECT_FLOAT_EQ(result100->samples[0], 1.0f);
}

TEST(SampleMapTest, NoteNotFoundReturnsNull) {
    synthcore::SampleMap map;
    
    synthcore::SampleRegion region;
    region.rootNote = 60;
    region.loNote = 60;
    region.hiNote = 60;
    
    map.addRegion(region);
    
    const auto* result = map.lookup(61, 64);
    EXPECT_EQ(result, nullptr);
}

TEST(SampleMapTest, RoundRobinCycling) {
    synthcore::SampleMap map;
    
    synthcore::SampleRegion rr1;
    rr1.rootNote = 60;
    rr1.loNote = 60;
    rr1.hiNote = 60;
    rr1.loVel = 0;
    rr1.hiVel = 127;
    rr1.rrGroup = 0;
    rr1.samples = {1.0f};
    
    synthcore::SampleRegion rr2;
    rr2.rootNote = 60;
    rr2.loNote = 60;
    rr2.hiNote = 60;
    rr2.loVel = 0;
    rr2.hiVel = 127;
    rr2.rrGroup = 1;
    rr2.samples = {2.0f};
    
    synthcore::SampleRegion rr3;
    rr3.rootNote = 60;
    rr3.loNote = 60;
    rr3.hiNote = 60;
    rr3.loVel = 0;
    rr3.hiVel = 127;
    rr3.rrGroup = 2;
    rr3.samples = {3.0f};
    
    map.addRegion(rr1);
    map.addRegion(rr2);
    map.addRegion(rr3);
    
    const auto* first = map.lookup(60, 64);
    const auto* second = map.lookup(60, 64);
    const auto* third = map.lookup(60, 64);
    const auto* fourth = map.lookup(60, 64);
    
    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);
    ASSERT_NE(third, nullptr);
    ASSERT_NE(fourth, nullptr);
    
    EXPECT_NE(first->samples[0], second->samples[0]);
    EXPECT_NE(second->samples[0], third->samples[0]);
    EXPECT_FLOAT_EQ(first->samples[0], fourth->samples[0]);
}

TEST(SampleMapTest, NoteRangeLookup) {
    synthcore::SampleMap map;
    
    synthcore::SampleRegion region;
    region.rootNote = 60;
    region.loNote = 57;
    region.hiNote = 62;
    region.samples = {0.5f};
    
    map.addRegion(region);
    
    EXPECT_NE(map.lookup(57, 64), nullptr);
    EXPECT_NE(map.lookup(60, 64), nullptr);
    EXPECT_NE(map.lookup(62, 64), nullptr);
    EXPECT_EQ(map.lookup(56, 64), nullptr);
    EXPECT_EQ(map.lookup(63, 64), nullptr);
}

TEST(SampleMapTest, ClearRemovesAllRegions) {
    synthcore::SampleMap map;
    
    synthcore::SampleRegion region;
    region.rootNote = 60;
    region.loNote = 60;
    region.hiNote = 60;
    
    map.addRegion(region);
    EXPECT_EQ(map.getRegionCount(), 1);
    
    map.clear();
    EXPECT_EQ(map.getRegionCount(), 0);
    EXPECT_EQ(map.lookup(60, 64), nullptr);
}
