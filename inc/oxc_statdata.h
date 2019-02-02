#ifndef _OXC_STATDATA_H
#define _OXC_STATDATA_H

#include <vector>
#include <limits>

#ifdef USE_OXC
  #include <oxc_devio.h>
  #include <oxc_floatfun.h>
  #define HOST_OSTREAM OutStream
#else
  #include <iostream>
  #define HOST_OSTREAM std::ostream
  #define NL "\n"
#endif



struct StatData {
  using sreal = float;
  // using sreal = double;
  struct Stat1 {
    sreal min = std::numeric_limits<sreal>::max(), max = std::numeric_limits<sreal>::min(),
          mean = 0, mean2 = 0, sd = 0;
    Stat1() = default;
  };
  struct StructPart {
    const sreal Stat1::* pptr;
    const char* const label;
  };
  std::vector<Stat1> d; // TODO: fixed with max ???
  unsigned n = 0;
  //
  explicit StatData( unsigned nch );
  auto getNch() const { return d.size(); }
  void add( const sreal *v );
  void reset();
  void calc();
  void out_part( HOST_OSTREAM &os, const sreal Stat1::* pptr, const char *lbl ) const;
  void out_parts( HOST_OSTREAM &os ) const;
  //
  static const constexpr StructPart structParts[] = {
    { &Stat1::mean,  "mean " },
    { &Stat1::mean2, "mean2" },
    { &Stat1::min,   "min  " },
    { &Stat1::max,   "max  " },
    { &Stat1::sd,    "sd   " }
  };
};

inline HOST_OSTREAM& operator<<( HOST_OSTREAM &os, const StatData &sd ) { sd.out_parts( os );  return os; } ;

#endif

