/*!
  \file

  \brief Implementation of class stir::PostsmoothingBackProjectorByBin

  \author Kris Thielemans
  \author Richard Brown

*/
/*
    Copyright (C) 2000- 2012, Hammersmith Imanet
    Copyright (C) 2019, University College London

    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/

#include "stir/recon_buildblock/PostsmoothingBackProjectorByBin.h"
#include "stir/DataProcessor.h"
#include "stir/DiscretisedDensity.h"
#include "stir/is_null_ptr.h"
#include "stir/warning.h"

START_NAMESPACE_STIR
const char * const 
PostsmoothingBackProjectorByBin::registered_name =
  "Post Smoothing";


void
PostsmoothingBackProjectorByBin::
set_defaults()
{
  original_back_projector_ptr.reset();
  _post_data_processor_sptr.reset();
}

void
PostsmoothingBackProjectorByBin::
initialise_keymap()
{
  parser.add_start_key("Post Smoothing Back Projector Parameters");
  parser.add_stop_key("End Post Smoothing Back Projector Parameters");
  parser.add_parsing_key("Original Back projector type", &original_back_projector_ptr);
  parser.add_parsing_key("filter type", &_post_data_processor_sptr);
}

bool
PostsmoothingBackProjectorByBin::
post_processing()
{
  if (is_null_ptr(original_back_projector_ptr))
  {
    warning("Pre Smoothing Back Projector: original back projector needs to be set");
    return true;
  }
  return false;
}

PostsmoothingBackProjectorByBin::
  PostsmoothingBackProjectorByBin()
{
  set_defaults();
}

PostsmoothingBackProjectorByBin::
PostsmoothingBackProjectorByBin(
                       const shared_ptr<BackProjectorByBin>& original_back_projector_ptr,
                       const shared_ptr<DataProcessor<DiscretisedDensity<3,float> > >& image_processor_ptr)
                       : original_back_projector_ptr(original_back_projector_ptr)
{
    _post_data_processor_sptr = image_processor_ptr;
}

PostsmoothingBackProjectorByBin::
~PostsmoothingBackProjectorByBin()
{}

void
PostsmoothingBackProjectorByBin::
set_up(const shared_ptr<const ProjDataInfo>& proj_data_info_ptr,
       const shared_ptr<const DiscretisedDensity<3,float> >& image_info_ptr)
{
  BackProjectorByBin::set_up(proj_data_info_ptr, image_info_ptr);
  original_back_projector_ptr->set_up(proj_data_info_ptr, image_info_ptr);
}

const DataSymmetriesForViewSegmentNumbers * 
PostsmoothingBackProjectorByBin::
get_symmetries_used() const
{
  return original_back_projector_ptr->get_symmetries_used();
}
#ifdef STIR_PROJECTORS_AS_V3
void 
PostsmoothingBackProjectorByBin::
actual_back_project(DiscretisedDensity<3,float>& density,
                    const RelatedViewgrams<float>& viewgrams, 
                    const int min_axial_pos_num, const int max_axial_pos_num,
                    const int min_tangential_pos_num, const int max_tangential_pos_num)
{
  if (!is_null_ptr(image_processor_ptr))
    {
      shared_ptr<DiscretisedDensity<3,float> > filtered_density_ptr
        (density.get_empty_discretised_density());
      assert(density.get_index_range() == filtered_density_ptr->get_index_range());
      original_back_projector_ptr->back_project(*filtered_density_ptr, viewgrams, 
                                                min_axial_pos_num, max_axial_pos_num,
                                                min_tangential_pos_num, max_tangential_pos_num);
      image_processor_ptr->apply(*filtered_density_ptr);
      density += *filtered_density_ptr;
    }
  else
    {
      original_back_projector_ptr->back_project(density, viewgrams, 
                                                min_axial_pos_num, max_axial_pos_num,
                                                min_tangential_pos_num, max_tangential_pos_num);
    }
}
#endif
void
PostsmoothingBackProjectorByBin::
actual_back_project(const RelatedViewgrams<float>& viewgrams,
                    const int min_axial_pos_num, const int max_axial_pos_num,
                    const int min_tangential_pos_num, const int max_tangential_pos_num)
{
      original_back_projector_ptr->back_project(viewgrams,
                                                min_axial_pos_num, max_axial_pos_num,
                                                min_tangential_pos_num, max_tangential_pos_num);
}

END_NAMESPACE_STIR
