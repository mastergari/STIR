
//
// $Id: BackProjectorByBinUsingSquareProjMatrixByBin.cxx
//

/*!
  \file
  \ingroup recon_buildblock

  \brief non-inline implementations for stir::BackProjectorByBinUsingSquareProjMatrixByBin
  
  \author Sanida Mustafovic
  \author Kris Thielemans
    
*/
/*
    Copyright (C) 2000- 2011, Hammersmith Imanet Ltd
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/


#include "stir/recon_buildblock/BackProjectorByBinUsingSquareProjMatrixByBin.h"
#include "stir/Viewgram.h"
#include "stir/RelatedViewgrams.h"
#include "stir/BasicCoordinate.h"
#include "stir/VoxelsOnCartesianGrid.h"
#include "stir/is_null_ptr.h"
#include "stir/error.h"

START_NAMESPACE_STIR

const char * const 
BackProjectorByBinUsingSquareProjMatrixByBin::registered_name =
  "Matrix Square";


void
BackProjectorByBinUsingSquareProjMatrixByBin::
set_defaults()
{
  this->proj_matrix_ptr.reset();
}


BackProjectorByBinUsingSquareProjMatrixByBin::
BackProjectorByBinUsingSquareProjMatrixByBin(  
    const shared_ptr<ProjMatrixByBin>& proj_matrix_ptr
    )		   
    : proj_matrix_ptr(proj_matrix_ptr)
  {
    assert(!is_null_ptr(proj_matrix_ptr));	 
    
  }

const DataSymmetriesForViewSegmentNumbers *
BackProjectorByBinUsingSquareProjMatrixByBin::get_symmetries_used() const
{
  if (!this->_already_set_up)
    error("BackProjectorByBin method called without calling set_up first.");
  return proj_matrix_ptr->get_symmetries_ptr();
}

void 
BackProjectorByBinUsingSquareProjMatrixByBin::
actual_back_project(DiscretisedDensity<3,float>& image,
		    const RelatedViewgrams<float>& viewgrams,
		    const int min_axial_pos_num, const int max_axial_pos_num,
		    const int min_tangential_pos_num, const int max_tangential_pos_num)
{
  ProjMatrixElemsForOneBin proj_matrix_row;
  
  
  RelatedViewgrams<float>::const_iterator r_viewgrams_iter = viewgrams.begin();

  
  while( r_viewgrams_iter!=viewgrams.end())
  {
    const Viewgram<float>& viewgram = *r_viewgrams_iter;
    const int view_num = viewgram.get_view_num();
    const int segment_num = viewgram.get_segment_num();
    
    for ( int tang_pos = min_tangential_pos_num ;tang_pos  <= max_tangential_pos_num ;++tang_pos)  
      for ( int ax_pos = min_axial_pos_num; ax_pos <= max_axial_pos_num ;++ax_pos)
      { 

	Bin bin(segment_num, view_num, ax_pos, tang_pos, viewgram[ax_pos][tang_pos]);
	proj_matrix_ptr->get_proj_matrix_elems_for_one_bin(proj_matrix_row, bin);
	ProjMatrixElemsForOneBin::iterator element_ptr = 
	  proj_matrix_row.begin();

	// square matrix elements
	while (element_ptr != proj_matrix_row.end())
	{	  
	  const float val=element_ptr->get_value();
	  *element_ptr *=val;	  
	   element_ptr++;
	}

	proj_matrix_row.back_project(image, bin);	
      }
      ++r_viewgrams_iter;
  }
}


void
BackProjectorByBinUsingSquareProjMatrixByBin::
initialise_keymap()
{
  parser.add_start_key("Back Projector ( Square) Using Matrix Parameters");
  parser.add_stop_key("End Back Projector ( Square) Using Matrix Parameters");
  parser.add_parsing_key("matrix type", &proj_matrix_ptr);
}

BackProjectorByBinUsingSquareProjMatrixByBin::
BackProjectorByBinUsingSquareProjMatrixByBin()
{
  set_defaults();
}


void
BackProjectorByBinUsingSquareProjMatrixByBin::
set_up(const shared_ptr<const ProjDataInfo>& proj_data_info_ptr,
       const shared_ptr<const DiscretisedDensity<3,float> >& image_info_ptr)

{
  BackProjectorByBin::set_up(proj_data_info_ptr, image_info_ptr);
  proj_matrix_ptr->set_up(proj_data_info_ptr, image_info_ptr);
}

END_NAMESPACE_STIR
