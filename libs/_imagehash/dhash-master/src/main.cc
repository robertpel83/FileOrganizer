/* compile with:
 *      g++ -g -Wall example.cc `pkg-config vips-cpp --cflags --libs`
 */

#include <cstdint>
#include <bitset>
#include <iostream>
#include <map>

#include <vips/vips8>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
using namespace vips;
using namespace std;

constexpr bool debug = false;

struct dim_t {
  int w;
  int h;
  int rot90;
};

// load & reduce image to w,h and convert to b&W
VImage reduce( const char* name,
               const dim_t& k,
               bool debug = false
    ) {

    int h = k.h, w = k.w;
    if( k.rot90) swap( h, w);

    // first stage thumbnail
    VImage thumb = VImage::thumbnail( name, w,
                                      VImage::option()
                                        ->set( "size", VIPS_SIZE_FORCE)
                                        ->set( "height", h)
                                        ->set( "no_rotate", true)
                                    );

    VImage cached = VImage::new_memory();
    thumb.write( cached);

    if( debug) cached.write_to_file ("thumb.ppm");

    VImage reduced = cached
                       .colourspace(VIPS_INTERPRETATION_sRGB, 
                                 VImage::option()
                                    ->set ("source_space", VIPS_INTERPRETATION_B_W)
                                  )
                    ;

    if( k.rot90) reduced = reduced.rot90();

    return reduced;
}

uint64_t dhash( VImage hash ) {
    auto w = hash.width();
    auto h = hash.height();
    //cout << "w: " << w << " h: " << h << "band: " << hash.bands() << "\n";
               
    auto* p = (uint8_t*) hash[0].data();          // this trigger the pipeline and take 20ms

    uint64_t hash_value = 0;

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            hash_value <<= 1;
            hash_value |= *p++ > 0 ? 1 : 0;
        }
    }

    return hash_value;
}

// find closes bound in map for given value
template<class T>
auto bound( int pos, map<int,T>& m) {

  auto lb = m.lower_bound( pos );
  std::pair<typename map<int,T>::iterator, typename map<int,T>::iterator> res;
  res.first = lb;
  res.second = lb;

  if( lb == m.end() && m.size() > 0) {
    // not found, we want to have the last element
    res.first = std::prev( m.end());
  } else if (lb == m.begin()) {
    // or first element
  } else {
    if (lb->first == pos)
      // value found in map
      ;
    else {
      // value not found in map, we want it to be bounded by existing value
        res.first = std::prev(lb);
    }
  }

  return res;
}

// find key dimension that are the closest to image aspect ratio
dim_t key( VImage from) {
  dim_t res = {};

  double r = (double) from.width() / from.height();

  if( r < 1.0) {
    res.rot90 = true;
    r = 1./r;
  }

  int ratio = r*100+0.5;
  if( debug)
    cerr << "ratio: " << ratio << "\n";

  // return matching hash mask that fits 64bits
  std::map<int, dim_t> m { {100, { 8, 8, 0}},
                           {130, { 9, 7, 0}},
                           {170, { 10, 6, 0}},
                       };

  auto p2 = bound( ratio, m);
  if( debug)
    std::cerr << "bound: [" << p2.first->first << "," << p2.second->first << "]\n";

  // find closest form factor
  int closest = p2.first->first;
  if( p2.second != m.end() 
      && abs( ratio - closest) > abs( p2.second->first - ratio))
    closest = p2.second->first;

  auto k = m[closest];
  res.w = k.w + 1;
  res.h = k.h;

  if( debug)
  cerr << "closest: " << closest 
       << " , w: "  << res.w 
       << ", h: "   << res.h 
       << " , rot90: " << res.rot90 
       << "\n";

  return res;

}


int
main (int argc, char **argv)
{ 

  if (VIPS_INIT (argv[0])) 
    vips_error_exit (NULL);

  if (argc < 1+1)
    vips_error_exit ("usage: %s file* ", argv[0]);

  int i = 1;
  for( const char* name=argv[i++];i<=argc;name=argv[i++]) {
  try {
    cerr << "file: " << name << "\n";

    VImage in = VImage::new_from_file (name );
    auto k = key( in);

    VImage reduced = reduce( name, k, debug );
    if( debug) reduced.write_to_file ("reduced.pgm");
    
    int h = k.h, w = k.w;
    VImage A = reduced.crop( 0, 0, w - 1, h );
    VImage B = reduced.crop( 1, 0, w - 1, h );

    if( debug) A.write_to_file ("A.pgm");
    if( debug) B.write_to_file ("B.pgm");

    VImage diff = A < B; 
    if( debug) diff.write_to_file ("diff.pgm");

    auto hash = dhash( diff );

    // JSON output
    json j = {
        {"hash", hash},
        {"rot90", k.rot90},
        {"width", diff.width() },
        {"height", diff.height() },
        {"file", name}
    };

    std::cout << j << "\n";

    if( debug)
      std::cerr << std::bitset<64>( hash) << "\n";
  } catch (vips::VError& e) {
    std::cerr << name << "\n - error: " << e.what() << "\n";

    // JSON output
    json j = {
        {"hash", 0},
        {"rot90",0},
        {"width", 0 },
        {"height", 0 },
        {"file", name}
    };

    std::cout << j << "\n";
  }
  }


  vips_shutdown ();

  return 0;
}