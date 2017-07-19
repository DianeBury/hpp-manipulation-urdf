// Copyright (c) 2014, LAAS-CNRS
// Authors: Joseph Mirabel (joseph.mirabel@laas.fr)
//
// This file is part of hpp-manipulation-urdf.
// hpp-manipulation-urdf is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-manipulation-urdf is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-manipulation-urdf. If not, see <http://www.gnu.org/licenses/>.

#include <hpp/util/debug.hh>

#include <boost/algorithm/string.hpp>

#include "hpp/manipulation/parser/factories/sequence.hh"

namespace hpp {
  namespace manipulation {
    namespace parser {
      namespace {
        struct StringIsEmpty : public std::unary_function<std::string, bool>{
          bool operator () (std::string s) const {return s.empty ();}
        };

        template <typename ValueType> bool cast (const std::string& str, ValueType* val)
        {
          hppDout (error, "Unkown type.");
          return false;
        }

        template <> bool cast <int> (const std::string& str, int* val)
        {
          if ( TIXML_SSCANF (str.c_str (), "%d", val) == 1 )
            return true;
          return false;
        }

        template <> bool cast <unsigned int> (const std::string& str, unsigned int* val)
        {
          if ( TIXML_SSCANF (str.c_str (), "%u", val) == 1 )
            return true;
          return false;
        }

        template <> bool cast <double> (const std::string& str, double* val)
        {
          if ( TIXML_SSCANF (str.c_str (), "%lf", val) == 1 )
            return true;
          return false;
        }

        template <> bool cast <float> (const std::string& str, float* val)
        {
          if ( TIXML_SSCANF (str.c_str (), "%f", val) == 1 )
            return true;
          return false;
        }

        template <> bool cast <bool> (const std::string& str, bool* val)
        {
          int iVal;
          if (cast <int> (str, &iVal)) {
            *val = (iVal == 0) ? false : true;
            return true;
          }
          if (str.compare ("true") == 0) {
            *val = true;
            return true;
          }
          if (str.compare ("false") == 0) {
            *val = false;
            return true;
          }
          return false;
        }
      }

      template <typename ValueType>
      void SequenceFactory<ValueType>::addTextChild (const XMLText* text)
      {
        values_.clear ();
        std::string t(text->Value ());
        typedef std::list<std::string> StringList;
        StringList values;

        boost::algorithm::split (values, t,
            boost::algorithm::is_any_of (" \n\t\r"),
            boost::algorithm::token_compress_on);
        values.remove_if (StringIsEmpty());
        if (size_ > 0 && values.size () != size_) {
          std::ostringstream oss;
          oss << "Wrong sequence size, expecting " << size_ << ", got "
              << values.size ();
          throw std::invalid_argument (oss.str ().c_str ());
        }

        ValueType v;
        for (StringList::const_iterator it = values.begin ();
            it != values.end (); it++) {
          if (!cast <ValueType> (*it, &v)) {
            std::ostringstream oss; oss << "Failed to cast string " << *it;
            throw std::invalid_argument (oss.str ().c_str ());
          }
          values_.push_back (v);
        }
      }

      template class SequenceFactory <bool>;
      template class SequenceFactory <int>;
      template class SequenceFactory <unsigned int>;
      template class SequenceFactory <double>;
      template class SequenceFactory <float>;
    } // namespace parser
  } // namespace manipulation
} // namespace hpp
