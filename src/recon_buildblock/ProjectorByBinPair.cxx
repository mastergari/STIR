//
//
/*!
  \file
  \ingroup projection

  \brief non-inline implementations for stir::ProjectorByBinPair
  
  \author Kris Thielemans
    
*/
/*
    Copyright (C) 2000- 2009, Hammersmith Imanet Ltd
    Copyright (C) 2018, University College London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/


#include "stir/recon_buildblock/ProjectorByBinPair.h"
#include "stir/ProjDataInfo.h"
#include "stir/DiscretisedDensity.h"
#include "stir/Succeeded.h"
#include "stir/error.h"
#include <boost/format.hpp>

START_NAMESPACE_STIR


ProjectorByBinPair::
ProjectorByBinPair()
  :   _already_set_up(false)
{
}

Succeeded
ProjectorByBinPair::
set_up(const shared_ptr<const ProjDataInfo>& proj_data_info_sptr,
       const shared_ptr<const DiscretisedDensity<3,float> >& image_info_sptr)

{    	 
  _already_set_up = true;
  _proj_data_info_sptr = proj_data_info_sptr->create_shared_clone();
  _density_info_sptr = image_info_sptr;
  forward_projector_sptr->set_up(proj_data_info_sptr, image_info_sptr);
  back_projector_sptr->set_up(proj_data_info_sptr, image_info_sptr);
  return Succeeded::yes;
}


void
ProjectorByBinPair::
check(const ProjDataInfo& proj_data_info, const DiscretisedDensity<3,float>& density_info) const
{
  if (!this->_already_set_up)
    error("ProjectorByBinPair method called without calling set_up first.");
  if (!(*this->_proj_data_info_sptr >= proj_data_info))
    error(boost::format("ProjectorByBinPair set-up with different geometry for projection data.\nSet_up was with\n%1%\nCalled with\n%2%")
          % this->_proj_data_info_sptr->parameter_info() % proj_data_info.parameter_info());
  if (! this->_density_info_sptr->has_same_characteristics(density_info))
    error("ProjectorByBinPair set-up with different geometry for density or volume data.");
}  

//ForwardProjectorByBin const * 
const shared_ptr<ForwardProjectorByBin>
ProjectorByBinPair::
get_forward_projector_sptr() const
{
  return forward_projector_sptr;
}

//BackProjectorByBin const * 
const shared_ptr<BackProjectorByBin>
ProjectorByBinPair::
get_back_projector_sptr() const
{
  return back_projector_sptr;
}

END_NAMESPACE_STIR
