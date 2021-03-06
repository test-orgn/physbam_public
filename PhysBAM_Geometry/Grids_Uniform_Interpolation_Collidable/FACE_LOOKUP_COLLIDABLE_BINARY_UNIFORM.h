#if 0
//#####################################################################
// Copyright 2009, Elliot English, Avi Robinson-Mosher.
// This file is part of PhysBAM whose distribution is governed by the license contained in the accompanying file PHYSBAM_COPYRIGHT.txt.
//#####################################################################
#ifndef __FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM__
#define __FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM__

#include <PhysBAM_Tools/Grids_Uniform_Arrays/FACE_ARRAYS_BINARY_UNIFORM.h>
#include <PhysBAM_Geometry/Grids_Uniform_Collisions/GRID_BASED_COLLISION_GEOMETRY_UNIFORM.h>
namespace PhysBAM{

template<class T_GRID,class T_NESTED_LOOKUP> // T_NESTED_LOOKUP=FACE_LOOKUP_BINARY_UNIFORM<T_GRID>
class FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM
{
    typedef typename T_GRID::VECTOR_T TV;typedef typename TV::SCALAR T;
    typedef typename T_GRID::VECTOR_INT TV_INT;typedef typename GRID_ARRAYS_POLICY<T_GRID>::FACE_ARRAYS T_FACE_ARRAYS;
    typedef typename GRID_ARRAYS_POLICY<T_GRID>::FACE_ARRAYS::template REBIND<bool>::TYPE FACE_ARRAYS_BOOL;typedef typename COLLISION_GEOMETRY_COLLECTION_POLICY<T_GRID>::GRID_BASED_COLLISION_GEOMETRY T_GRID_BASED_COLLISION_GEOMETRY;
    typedef typename T_GRID::CELL_ITERATOR CELL_ITERATOR;
public:
    template<class T_NESTED_LOOKUP_2> struct REBIND_NESTED_LOOKUP{typedef FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM<T_GRID,T_NESTED_LOOKUP_2> TYPE;};
    typedef T ELEMENT;
    typedef T_NESTED_LOOKUP NESTED_LOOKUP;

    class LOOKUP;

    const T_NESTED_LOOKUP& nested_face_lookup;
    const T_GRID_BASED_COLLISION_GEOMETRY& body_list;
    const FACE_ARRAYS_BOOL* valid_value_mask;

    FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM(const T_NESTED_LOOKUP& nested_face_lookup_input,const T_GRID_BASED_COLLISION_GEOMETRY& body_list_input,
        const FACE_ARRAYS_BOOL* valid_value_mask_input);

    const T_FACE_ARRAYS& Raw_Data() const
    {return nested_face_lookup.Raw_Data();}

    const T_NESTED_LOOKUP& Nested() const
    {return nested_face_lookup;}

    LOOKUP Starting_Point_Face(const int side,const int axis,const TV_INT& face) const
    {// do a ray-cast to find which cell center is visible
        // if both are visible, need to know that (binary).  Reflect values so averaging gets it right?
        TV_INT adjacent_cell_center=side&1?face-TV_INT::Axis_Vector(axis):face;
        bool both_neighbors_visible=body_list.cell_neighbors_visible(face-TV_INT::Axis_Vector(axis))(axis) && (*valid_value_mask)(axis,face);// && !body_list.Latest_Cell_Crossover(adjacent_cell_center.;
        return LOOKUP(*this,nested_face_lookup.Starting_Point_Face(axis,face),adjacent_cell_center,both_neighbors_visible,axis);}
    
    LOOKUP Starting_Point_Cell(const TV_INT& cell) const
    {return LOOKUP(*this,nested_face_lookup.Starting_Point_Cell(cell),cell);}

    class LOOKUP
    {
    private:
        const FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM<T_GRID,T_NESTED_LOOKUP>& face_lookup;
        typename T_NESTED_LOOKUP::LOOKUP nested_lookup;
    public:
        typedef T ELEMENT;
        mutable bool reference_point_set;
        mutable bool reference_point_inside;
        mutable TV_INT reference_point_cell;
        mutable bool found_valid_point;
        mutable TV reference_point;
        mutable TV object_velocity;
        mutable TV_INT cells_in_block[T_GRID::number_of_cells_per_block];
        mutable TV_INT cell;
        mutable TV offset;
        bool both_cells_visible;
        int reference_point_axis;

        LOOKUP(const FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM<T_GRID,T_NESTED_LOOKUP>& face_lookup_input,const typename T_NESTED_LOOKUP::LOOKUP& nested_lookup_input,
            int reference_point_axis_input=0);
        LOOKUP(const FACE_LOOKUP_COLLIDABLE_BINARY_UNIFORM<T_GRID,T_NESTED_LOOKUP>& face_lookup_input,const typename T_NESTED_LOOKUP::LOOKUP& nested_lookup_input,const TV_INT& cell_input,
            const bool both_cells_visible_input=true,const int reference_point_axis_input=0);

        int Number_Of_Ghost_Cells() const
        {return nested_lookup.Number_Of_Ghost_Cells();}

        void Set_Reference_Point(const TV& reference_point_input) const;

        void Clear_Reference_Point() const
        {assert(reference_point_set);reference_point_set=false;reference_point_inside=false;}

        T V_face(const int side,const int axis,const TV_INT& face) const
        {return (*(ARRAY<T,SIDED_FACE_INDEX<TV::dimension> >*)(&nested_lookup.V_face))(side,axis,face);}

        T operator()(const int axis,const TV_INT& face) const;

        /*T operator()(const int axis,const TV_INT& face) const
        {T face_velocity=0;assert(reference_point_set);
        if(reference_point_inside){found_valid_point=true;return object_velocity[axis];}
        else if(face_lookup.body_list.Face_Velocity(axis,face,cells_in_block,T_GRID::number_of_cells_per_block,reference_point,face_velocity)){found_valid_point=true;return face_velocity;}
        else{
            if(face_lookup.valid_value_mask && !(*face_lookup.valid_value_mask).Component(axis)(face)) return 0;
            found_valid_point=true;return nested_lookup(axis,face);}}*/
    };

//#####################################################################
};
}
#endif
#endif
