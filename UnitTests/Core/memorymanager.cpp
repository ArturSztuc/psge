#include <gtest/gtest.h>
#include <Core/Memory/Allocator.hpp>

struct Datum
{
  double  a;
  float   b;
  int     c;
  bool    d;
};

struct BigData
{
  double  a;
  float   b;
  int     c;
  bool    d;
  double  e;
  float   f;
  int     g;
  bool    h;
};

TEST(MemoryManagerTests, PoolAllocator)
{

  PoolAllocator<Datum> allocator;

  EXPECT_EQ(allocator.GetAllocationCount(), 0);
  EXPECT_EQ(allocator.GetAllocationSize(), 0);

  EXPECT_EQ(allocator.GetFreeAllocationCount(), 1024);
  EXPECT_EQ(allocator.GetFreeAllocationSize(), 1024 * sizeof(Datum));

  Datum *data = allocator.Allocate();
  data->a = 5.5;
  data->b = 5.5;
  data->c = 5;
  data->d = true;

  EXPECT_EQ(allocator.GetAllocationCount(), 1);
  EXPECT_EQ(allocator.GetAllocationSize(), 1 * sizeof(Datum));

  EXPECT_EQ(allocator.GetFreeAllocationCount(), 1023);
  EXPECT_EQ(allocator.GetFreeAllocationSize(), 1023 * sizeof(Datum));

  allocator.Deallocate(data);

  EXPECT_EQ(allocator.GetAllocationCount(), 0);
  EXPECT_EQ(allocator.GetAllocationSize(), 0 * sizeof(Datum));

  EXPECT_EQ(allocator.GetFreeAllocationCount(), 1024);
  EXPECT_EQ(allocator.GetFreeAllocationSize(), 1024 * sizeof(Datum));
}

TEST(MemoryManagerTests, MemoryManager)
{
  Datum *datum1 = MEMALLOC().Allocate<Datum>();
  Datum *datum2 = MEMALLOC().Allocate<Datum>();
  Datum *datum3 = MEMALLOC().Allocate<Datum>();

  BigData *datoha1 = MEMALLOC().Allocate<BigData>();
  BigData *datoha2 = MEMALLOC().Allocate<BigData>();

  double *d1 = MEMALLOC().Allocate<double>();
  double *d2 = MEMALLOC().Allocate<double>();
  double *d3 = MEMALLOC().Allocate<double>();
  double *d4 = MEMALLOC().Allocate<double>();

  EXPECT_EQ(MEMALLOC().GetAllocatorTypes(), 3);
  EXPECT_EQ(MEMALLOC().GetAllocationCount<Datum>(), 3);
  EXPECT_EQ(MEMALLOC().GetAllocationCount<BigData>(), 2);
  EXPECT_EQ(MEMALLOC().GetAllocationCount<double>(), 4);
}
