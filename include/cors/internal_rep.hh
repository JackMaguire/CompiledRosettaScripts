#pragam once

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

template< class T >
using Ptr< T > = std::shared_ptr< T >;

template< class T >
using make_ptr< T > = std::make_shared< T >;

template< class T >
using WeakPtr< T > = std::weak_ptr< T >;

namespace comprs {

enum class Type : unsigned char {
  UNKNOWN = 0,

  SCORE_FUNCTION,
  RESIDUE_SELECTOR,
  TASK_OPERATION,
  RES_LVL_TASK_OP,
  MOVE_MAP_FACTORY,
  SIMPLE_METRIC,
  MOVER,
  FILTER,
  
  RAW_STR,
  ARRAY,

  _count //keep this last
};

static std::vector< std::string > ScopeNameForType({
    "UNKNOWN",
    "SCORE_FUNCTIONS",
    "RESIDUE_SELECTOR",
    "TASKOPERATIONS",
    "RESIDUE_LEVEL_TASK_OPERATIONS",
    "MOVE_MAP_FACTORIES",
    "SIMPLE_METRICS",
    "MOVERS",
    "FILTERS",
    "XXX",
    "YYY",
    "ZZZ"
});

bool is_Rosetta_type( Type const t ){
  return t > Type::UNKNOWN and t < Type::RAW_STR;
}

class Element {
  Type type_ = UNKNOWN;
  std::unordered_map< std::string, Ptr< Element > > tokens_;
  std::vector< Ptr< Element > > subelements_;
  std::vector< std::string > comments_;

  std::string impl_type; //i.e., "FastDesign"

  void init(){
    tokens_.max_load_factor( 0.2 );
  }

public: //parsing
  Element(){
    init();
  }

  Element( Type const & t ):
    type_( t )
  {
    init();
  }

  void add_token(
    std::string const & key,
    Ptr< Element > const & value
  ){
    tokens_[ key ] = value;
  }

public: //compiling
  std::string
  to_string( std::string const & indentation ) const {
    std::stringstream ss;

    if( !comments_.empty() ){
      ss << '\n';
      for( std::string const & comment : comments_ )
	ss << indentation << "<!-- " << comment << "-->\n"
    }

    ss << 
  }
};

}// namespace comprs
