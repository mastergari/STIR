/*
    Copyright (C) 2003 - 2011-06-24, Hammersmith Imanet Ltd
    Copyright (C) 2011-07-01 - 2014, Kris Thielemans
    Copyright (C) 2019, National Physical Laboratory
    Copyright (C) 2019, University College of London
    This file is part of STIR.

    SPDX-License-Identifier: Apache-2.0

    See STIR/LICENSE.txt for details
*/
/*!
  \file
  \ingroup listmode
  \brief Declaration of class stir::ListModeData

  \author Daniel Deidda
  \author Kris Thielemans
*/

#ifndef __stir_listmode_ListModeData_H__
#define __stir_listmode_ListModeData_H__

#include <string>
#include <ctime>
#include "stir/ProjDataInfo.h"
#include "stir/ExamData.h"
#include "stir/RegisteredParsingObject.h"
#include "stir/listmode/ListRecord.h"
#include "stir/error.h"
# ifdef BOOST_NO_STDC_NAMESPACE
namespace std { using ::time_t; }
#endif

START_NAMESPACE_STIR
class ListRecord;
class Succeeded;
class ExamInfo;

/*!
  \brief The base class for reading list mode data.
  \ingroup listmode

  \par What is list mode data?

  List mode data is a format for storing detected counts as a list, as
  opposed to a histogram. For each count, a list mode event contains
  all the properties that the scanner can give you. Hence, the
  list mode data gives you all the information about your acquisition
  that you can possibly get. Which information this is obviously depends
  on the scanner.

  For most (all?) scanners, events are stored in chronological order.
  In addition to events, time flags are inserted into the list mode data.
  So, generally list mode data is a list of 'records', which can be
  of different types. In STIR, this concept of a 'record' corresponds to
  the ListRecord class and its relatives (see the documentation for
  ListRecord).

  \par Usage

  For most applications, i.e. when one just wants to go through the list of
  all events, the code would be as follows:

  \code
  shared_ptr<ListModeData>
    lm_data_sptr(read_from_file<ListModeData>(filename));

  // get a pointer to a 'template' record that will work for the scanner
  // from which we're reading data
  shared_ptr <ListRecord> record_sptr =
    lm_data_ptr->get_empty_record_sptr();
  // give the record a simple name to avoid cluttering the code below
  ListRecord& record = *record_sptr;

  double current_time = 0;
  while (lm_data_sptr->get_next_record(record) == Succeeded::yes)
    {
      if (record.is_time())
      {
    current_time= record.time().get_time_in_secs();
      }
      if (record.is_event())
      {
        if (record.event().is_prompt())
    { // do something
        }
        // ...
      }
    }
  \endcode

  In addition, there is a facility to 'remember' positions in the list,
  and go back to one of these positions. This could be useful to
  mark time frames. This goes as follows.

  \code
  // somehow I found out this is the start of the frame, so save it
  ListModeData::SavedPosition start_of_this_frame =
     lm_data_sptr->save_get_position();

  // now do something with this frame

  // now get back to the start of the frame
  if ( lm_data_sptr->set_get_position(start_of_this_frame)
     != Succeeded::yes)
    error("Help!");
  \endcode

  Currently, this class (and ListRecord) is generic for emission modalities
  such as PET and  SPECT.

  \par Notes for developers

  If you want to add a new type of list mode data, you have to make corresponding
  derived classes of ListModeData, ListRecord etc. You also have to modify
  make sure that read_from_file<ListModeData> recognises your data. This
  normally involves creating a new InputFileFormat class.
*/
class ListModeData : public ExamData
{
public:
  //! typedef used by read_from_file
  typedef ListModeData hierarchy_base_type;

  //! Use this typedef for save/set_get_position
  typedef unsigned int SavedPosition;

  //! Default constructor
  ListModeData();

  virtual
    ~ListModeData();

  //! Returns the name of the list mode data
  /*! This name is not necessarily unique, and might be empty. However, it is expected
      (but not guaranteed) that
      <code>ListModeData::read_from_file(lm_data_ptr-\>get_name())</code> would read
      the same list mode data.

      The reason this cannot be guaranteed is largely in case the list mode data is
      not really on disk, but the object corresponds for instance to a Monte Carlo simulator.
  */
  virtual std::string
    get_name() const = 0;

//  //! Get const pointer to exam info
//  const ExamInfo*
//    get_exam_info_ptr() const;
//  //! Get shared pointer to exam info
//  /*! \warning Use with care. If you modify the object pointer to by a shared ptr, all objects using the same
//    shared pointer will be affected. */
//  shared_ptr<ExamInfo>
//    get_exam_info_sptr() const;

#if 0
  //! Scan start time
  /*! In secs since midnight (UTC) 1/1/1970 (as returned by std::time()).

     Should return <tt>std::time_t(-1)</tt> if unknown or invalid.
     \deprecated
     Use ExamInfo instead.
  */
  virtual
    std::time_t
    get_scan_start_time_in_secs_since_1970() const;
#endif

  //! Get a pointer to an empty record
  /*! This is mainly/only useful to get a record of the correct type, that can then be
      passed to get_next_record().
  */

    shared_ptr <ListRecord> get_empty_record_sptr() const
    {return this->get_empty_record_helper_sptr();}

  //! Gets the next record in the listmode sequence
  virtual
    Succeeded get_next_record(ListRecord& event) const
    {      return get_next(event);}

  //! Call this function if you want to re-start reading at the beginning.
  virtual
    Succeeded reset() = 0;

  //! Save the current reading position
  /*!
      Note that the return value is not related to the number of events
      already read. In particular, you cannot do any arithmetic on it to
      skip a few events. This is different from e.g. std::streampos.

      \warning There is a maximum number of times this function can be called.
      This is determined by the SavedPosition type. Once you save more
      positions, the first positions will be overwritten. There is currently
      no way of finding out after how many times this will happen (but it's
      a large number...).

      \warning These saved positions are only valid for the lifetime of the
      ListModeData object.

      \warning A derived class might disable this facility. It will/should
      then always return Succeeded::no when calling set_get_position().
  */
  virtual
    SavedPosition save_get_position() = 0;

  //! Set the position for reading to a previously saved point

  virtual
    Succeeded set_get_position(const SavedPosition&) = 0;

  //! Get scanner pointer
  /*! Returns a pointer to a scanner object that is appropriate for the
      list mode data that is being read.
      \warning This member is obsolete and might be removed soon.
  */
  virtual const Scanner* get_scanner_ptr() const ;

  //! Return if the file stores delayed events as well (as opposed to prompts)
  virtual bool has_delayeds() const = 0;
  //! Returns the total number of events in the listmode file.
  //! \warning This function currently works only with ROOT input files. By default
  //! it will throw an error.
  virtual inline unsigned long int get_total_number_of_events() const
  {
      error("ListModeData: The function get_total_number_of_events() is currently not supported for this file format.");
      return 0;
  }

  virtual shared_ptr<const ProjDataInfo> get_proj_data_info_sptr() const ;

protected:
  virtual shared_ptr <ListRecord> get_empty_record_helper_sptr() const = 0;
  virtual Succeeded get_next(ListRecord& event) const = 0;
  virtual void set_proj_data_info_sptr(shared_ptr<const ProjDataInfo>) ;
  //! Has to be set by the derived class
  //  shared_ptr<ExamInfo> exam_info_sptr;
  //! Has to be initialised by the derived class
  shared_ptr<const ProjDataInfo> proj_data_info_sptr;
};

END_NAMESPACE_STIR

#endif
