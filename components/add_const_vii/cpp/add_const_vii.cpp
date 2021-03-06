/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file 
 * distributed with this source distribution.
 * 
 * This file is part of GNUHAWK.
 * 
 * GNUHAWK is free software: you can redistribute it and/or modify is under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * GNUHAWK is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "add_const_vii.h"


//
//   This file contains the bindings to the hosted block
//

//
//    For GNU Radio blocks with getter and setter methods, the associated property can be bound
//    directly to the block's getter and/or setter.
//
//    void component_class_i::createBlock()
//    {
//        ...
//        this->registerGetter("property_x", &gnuradio_class::getter_for_x);
//        this->registerSetter("property_y", &gnuradio_class::setter_for_y);
//        this->registerGetterSetter("property_z", &gnuradio_class::getter_for_z,
//                                   &gnuradio_class::setter_for_z);
//        ...
//    }

//
//    To bind to a setter via the REDHAWK component class
//
//    void component_class_i::setterForPropertyX()
//    {
//       if ( validGRBlock() ) {
//           gr_sptr->set_property_x(this->property_x);
//        }
//    }
//
//    void component_class_i::createBlock()
//    {
//        ...
//        this->setPropertyChangeListener("property_x", this, &component_class_i::setterForPropertyX);
//        ...
//    }

//
//    To bind to a getter via the REDHAWK component class
//
//    double component_class_i::getterForPropertyX()
//    {
//       if ( validGRBlock() ) {
//         return this->gr_sptr->get_property_x();
//       }
//       return 0.0;
//    }
//    void component_class_i::createBlock()
//    {
//        ...
//        this->registerGetValue("property_x", this, &component_class_i::getterForPropertyX);
//        ...
//    }


add_const_vii_i::add_const_vii_i(const char *uuid, const char *label) :
add_const_vii_base(uuid, label)
{
}

add_const_vii_i::~add_const_vii_i()
{
}

class int_to_long: std::unary_function< int, CORBA::Long > {

public:
  result_type operator()( argument_type i ) {
    return (result_type)i;
  }

};

class long_to_int: std::unary_function< CORBA::Long, int > {

public:
  result_type operator()( argument_type i ) {
    return (result_type)i;
  }

};

std::vector< CORBA::Long > add_const_vii_i::getK() {

  std::vector< CORBA::Long > out_k;
  if ( validGRBlock() ) {

    std::vector<int> t_k;
    t_k = gr_sptr->k();
    out_k.resize(t_k.size());
    std::transform( t_k.begin(), t_k.end(), out_k.begin(), int_to_long() );
  }

  return out_k;
}
void add_const_vii_i::setK( const std::string &id ) {
    if ( validGRBlock() ) {
      std::vector<int> t_k( k.begin(), k.end() );
      gr_sptr->set_k( t_k );
    }
}



// createBlock
//
// Create the actual GNU Radio Block to that will perform the work method. The resulting
// block object is assigned to gr_stpr
//
// Add property change callbacks for getter/setter methods
//
void add_const_vii_i::createBlock()
{

  // need to have at least 1 item in the list... bad things happend if not
  if ( k.size() < 1 )  return;

  try {
    std::vector<int> t_k;
    t_k.resize( k.size() );
    std::transform( k.begin(), k.end(), t_k.begin(), long_to_int() );
    gr_sptr = gr_make_add_const_vii( t_k );
  }
  catch(...) {
    return;
  }


  this->setPropertyChangeListener("k", this, &add_const_vii_i::setK );
  this->registerGetValue("k", this, &add_const_vii_i::getK);
  //this->registerGetterSetter("k", &gr_add_const_vii::k, &gr_add_const_vii::set_k);

  // 
  // Use setThrottle method to enable the throttling of consumption/production of data by the
  // service function. The affect of the throttle will try to pause the execution of the 
  // service function for a specified duration.  This duration is calculated using the getTargetDuration() method
  // and the actual processing time to perform the work method.
  //
  // This is turned ON by default for "output" only components
  //
  // setThrottle( bool onoff );

  // 
  // Use maintainTimeStamp to enable proper data flow of timestamp with input and output data. 
  // if turned on (true)
  //  The timestamp from the input source will be used and maintained based on the output rate and
  //  the number of samples produced
  // if turned off
  //   then the timestamp from the input source is passed through if available or the time of day
  // 
  // maintainTimestamp( bool onoff );

  setupIOMappings();
} 


//
// createOutputSRI
//
// For each output mapping defined, a call to createOutputSRI will be issued with the associated output index.
// This default SRI and StreamID will be saved to the mapping and pushed down stream via pushSRI.
//
// @param idx : output stream index number to associate the returned SRI object with
// @return sri : default SRI object passed down stream over a RedHawk port
//      
BULKIO::StreamSRI add_const_vii_i::createOutputSRI( int32_t idx)
{
  //
  // idx is the stream number that you are returning an SRI context for
  //

  BULKIO::StreamSRI sri = BULKIO::StreamSRI();
  sri.hversion = 1;
  sri.xstart = 0.0;
  sri.xdelta = 1;
  sri.xunits = BULKIO::UNITS_TIME;
  sri.subsize = 0;
  sri.ystart = 0.0;
  sri.ydelta = 0.0;
  sri.yunits = BULKIO::UNITS_NONE;
  sri.mode = 0;
  std::ostringstream t;
  t << naming_service_name.c_str() << "_" << idx;
  std::string sid = t.str();
  sri.streamID = CORBA::string_dup(sid.c_str());
  
  return sri;
 
} 


