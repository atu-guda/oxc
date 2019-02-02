#include <cmath>

#include <oxc_statdata.h>

using namespace std;

StatData::StatData( unsigned nch )
{
  d.assign( nch, Stat1() );
}

void StatData::reset()
{
  d.assign( d.size(), Stat1() );
  n = 0;
}

void StatData::add( const sreal *v )
{
  for( decltype(+d.size()) j=0; j<d.size(); ++j ) {
    auto cv = v[j];
    auto cv2 = cv * cv;
    if( cv < d[j].min ) { d[j].min = cv; }
    if( cv > d[j].max ) { d[j].max = cv; }
    d[j].mean  = ( n * d[j].mean  + cv  ) / (n+1);
    d[j].mean2 = ( n * d[j].mean2 + cv2 ) / (n+1);
  }
  ++n;
}

void StatData::calc()
{
  for( auto &t : d ) {
    t.sd  = sqrt(  t.mean2  - t.mean * t.mean );
  }
}

void StatData::out_part( HOST_OSTREAM &os, const sreal Stat1::* pptr, const char *lbl ) const
{
  os << NL "# " << lbl << "   ";
  for( decltype(+d.size()) j=0; j<d.size(); ++j ) {
    os << ' ' << d[j].*pptr;
  }
}

void StatData::out_parts( HOST_OSTREAM &os ) const
{
  os << NL "# n_real= " << n << " n_ch = " << d.size();
  for( auto p : structParts ) {
    out_part( os, p.pptr, p.label );
  }
}


