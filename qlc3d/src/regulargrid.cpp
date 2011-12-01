<<<<<<< HEAD
#include <regulargrid.h>

const unsigned int RegularGrid::NOT_AN_INDEX = std::numeric_limits<unsigned int>::max();

RegularGrid::RegularGrid():
    nx_(0), ny_(0), nz_(0),
    npr_(0),
    dx_(0), dy_(0), dz_(0)

{
    xLimits_[0] = 0; xLimits_[1] = 0;
    yLimits_[0] = 0; yLimits_[1] = 0;
    zLimits_[0] = 0; zLimits_[1] = 0;

}

bool RegularGrid::createFromTetMesh(const int &nx, const int &ny, const int &nz,
                                    Geometry &geom)
{
// CREATES INTEPOLATION TABLE FROM A TETRAHEDRAL MESH DESCRIBED BY geom
// SO THAT FAST INTEPOLATION CAN BE PERFORMED LATER ON

    xLimits_[0] = geom.getXmin();   xLimits_[1] = geom.getXmax();
    yLimits_[0] = geom.getYmin();   yLimits_[1] = geom.getYmax();
    zLimits_[0] = geom.getZmin();   zLimits_[1] = geom.getZmax();

    // LIMIT MIN NUMBER OF NODES TO 1 PER DIMENSION
    nx_ = nx == 0 ? 1 : nx;
    ny_ = ny == 0 ? 1 : ny;
    nz_ = nz == 0 ? 1 : nz;

    npr_ = nx_*ny_*nz_;

    // SPECIAL CASE, WHEN ONLY A SINGLE NODE IN A DIRECTION IS REQUIRED -> di = 0
    if (nx == 1 )
        dx_ = ( xLimits_[1] - xLimits_[0]) / ( 2.0 ); // <--still incorrect.see getGridX
    else
        dx_ = ( xLimits_[1] - xLimits_[0] ) / ( nx_ -1 );

    if (ny <= 1 )
        dy_ = ( yLimits_[1] - yLimits_[0]) / ( 2.0 );
    else
        dy_ = ( yLimits_[1] - yLimits_[0] ) / ( ny_ - 1 );

    if (nz <= 1)
        dz_ = ( zLimits_[1] - zLimits_[0]) / ( 2.0 );
    else
        dz_ = (zLimits_[1] - zLimits_[0] ) / ( nz_ - 1 );

    generateLookupList(geom);

    return true;
}


bool RegularGrid::generateLookupList(Geometry &geom)
{
// FILLS IN VALUES FOR THE INTEPOLATION LOOKUP TABLES

    double* coords = new double[npr_*3]; // allocate temporary memory for regular grid coordinates

    size_t cc = 0;    // coordinate counter
    for (unsigned int k = 0 ; k < nz_ ; k++ )// loop over z
    {
        double z = getGridZ(k);
        for (unsigned int j = 0 ; j < ny_ ; j++ ) // loop over y
        {
            double y = getGridY( j );
            for ( unsigned int i = 0 ; i < nz_ ; i++, cc++) // loop over x
            {
                double x = getGridX( j );

                coords[3*cc + 0 ] = x;//getGridX( i );
                coords[3*cc + 1 ] = y;//getGridY( j );
                coords[3*cc + 2 ] = z;//getGridY( k );
            }
        }// end loop over y
    }// end loop over z

// GENERATE INDEX TO TETS THAT CONTAIN EARCH REGULAR GRID POINT
// INDEX WILL HAVE SPECIAL VALUE Geom::NOT_AN_INDEX, IF POITN WAS NOT FOUND
// THIS MAY HAPPEN WHEN THE UNDERLYING TET MESH IS NOT A QUBE
    std::vector< unsigned int > indT; // index to tet containing a regular coordinate
    geom.genIndToTetsByCoords(indT,
                              coords,
                              npr_,
                              false); // do NOT terminate app if a coord is not found


// NOW CALCULATE WEIGHTS AND NODE INDEXES FOR EACH REGULAR GRID POINT
    lookupList.clear();
    lookupList.reserve( npr_ );
    Mesh* t = geom.t;   // TETRAHEDRAL MESH. OBVIOUSLY THIS IS EVIL...
    double* p = geom.getPtrTop();
    for (unsigned int i = 0; i < npr_ ; i++)
    {

        lookup lu; // NEW LOOKUP TABLE ENTRY
        if ( indT[i] != Geometry::NOT_AN_INDEX ) // IF CONTAINING TET ELEMENT WAS FOUND
        {
            double* coord = &coords[3*i]; // pointer to coordinates of i'th regular point
            // SET INDEXES TO NEIGHBOURING VERTEXES
            lu.ind[0] = t->getNode( indT[i], 0 );
            lu.ind[1] = t->getNode( indT[i], 1 );
            lu.ind[2] = t->getNode( indT[i], 2 );
            lu.ind[3] = t->getNode( indT[i], 3 );

            // CALCULATE NEIGHBOUR NODE WEIGHTS - THESE ARE THE LOCAL COORDINATES
            // OF THE VERTEXES OF THE TET CONTAINING THE REGULAR POINT
            t->CalcLocCoords( indT[i], p, coord, &lu.weight[0] );
        }
        else // CONTAINING TET ELEMENT WAS NOT FOUND
        {
            lu.ind[0] = NOT_AN_INDEX; lu.ind[1] = NOT_AN_INDEX;
            lu.ind[2] = NOT_AN_INDEX; lu.ind[3] = NOT_AN_INDEX;
            lu.weight[0] = 0; lu.weight[1] = 0;
            lu.weight[2] = 0; lu.weight[3] = 0;
        }
        lookupList.push_back( lu );
    }


    delete [] coords;
    return true;


}

void RegularGrid::interpolateToRegular(const double*& sclrIn,
                                       double*& sclrOut)
{
    // INTERPOLATES FROM TET MESH TO REGULAR GRID
    if (!npr_)
    {
        printf("error in %s, Regular grid doesn't seem to be initialised - bye!\n", __func__);
        exit(1);
    }

    for ( size_t i = 0 ; i < lookupList.size() ; i ++)
    {
        lookup L = lookupList[i];
        double s0 = sclrIn[ L.ind[0] ];
        double s1 = sclrIn[ L.ind[1] ];
        double s2 = sclrIn[ L.ind[2] ];
        double s3 = sclrIn[ L.ind[3] ];

        sclrOut[i] = L.weight[0]*s0 + L.weight[1]*s1 + L.weight[2]*s2 + L.weight[3]*s3;
    }

}


bool RegularGrid::writeVTKGrid(const char* filename,
                               const double *sclrIn)
{


    double* sclrR = new double[ npr_ ];

    interpolateToRegular( sclrIn,
                          sclrR);

    std::fstream fid;
    fid.open( filename , std::fstream::out);




    if ( !fid.is_open() )
        return false;

    vtkIOFun::writeID( fid );
    vtkIOFun::writeHeader( fid, "header string");
    vtkIOFun::writeFileFormat(fid, vtkIOFun::FileFormat(vtkIOFun::ASCII) );
    vtkIOFun::writeDatasetFormat(fid, nx_, ny_, nz_,
                                 0.,0.0,0.,
                                 dx_, dy_, dz_);

    vtkIOFun::writeScalarData( fid , npr_, "potential", sclrR);

    fid.close();
    delete [] sclrR;
    return true;
}



=======
#include <regulargrid.h>

const unsigned int RegularGrid::NOT_AN_INDEX = std::numeric_limits<unsigned int>::max();

RegularGrid::RegularGrid():
    nx_(0), ny_(0), nz_(0),
    npr_(0),
    dx_(0), dy_(0), dz_(0)

{
    xLimits_[0] = 0; xLimits_[1] = 0;
    yLimits_[0] = 0; yLimits_[1] = 0;
    zLimits_[0] = 0; zLimits_[1] = 0;

}

bool RegularGrid::createFromTetMesh(const int &nx, const int &ny, const int &nz,
                                    Geometry &geom)
{
// CREATES INTEPOLATION TABLE FROM A TETRAHEDRAL MESH DESCRIBED BY geom
// SO THAT FAST INTEPOLATION CAN BE PERFORMED LATER ON

    xLimits_[0] = geom.getXmin();   xLimits_[1] = geom.getXmax();
    yLimits_[0] = geom.getYmin();   yLimits_[1] = geom.getYmax();
    zLimits_[0] = geom.getZmin();   zLimits_[1] = geom.getZmax();

    // LIMIT MIN NUMBER OF NODES TO 1 PER DIMENSION
    nx_ = nx == 0 ? 1 : nx;
    ny_ = ny == 0 ? 1 : ny;
    nz_ = nz == 0 ? 1 : nz;

    npr_ = nx_*ny_*nz_;

    // SPECIAL CASE, WHEN ONLY A SINGLE NODE IN A DIRECTION IS REQUIRED -> di = 0
    if (nx == 1 )
        dx_ = ( xLimits_[1] - xLimits_[0]) / ( 2.0 ); // <--still incorrect.see getGridX
    else
        dx_ = ( xLimits_[1] - xLimits_[0] ) / ( nx_ -1 );

    if (ny <= 1 )
        dy_ = ( yLimits_[1] - yLimits_[0]) / ( 2.0 );
    else
        dy_ = ( yLimits_[1] - yLimits_[0] ) / ( ny_ - 1 );

    if (nz <= 1)
        dz_ = ( zLimits_[1] - zLimits_[0]) / ( 2.0 );
    else
        dz_ = (zLimits_[1] - zLimits_[0] ) / ( nz_ - 1 );

    generateLookupList(geom);

    return true;
}


bool RegularGrid::generateLookupList(Geometry &geom)
{
// FILLS IN VALUES FOR THE INTEPOLATION LOOKUP TABLES

    double* coords = new double[npr_*3]; // allocate temporary memory for regular grid coordinates

    size_t cc = 0;    // coordinate counter
    for (unsigned int k = 0 ; k < nz_ ; k++ )// loop over z
    {
        double z = getGridZ(k);
        for (unsigned int j = 0 ; j < ny_ ; j++ ) // loop over y
        {
            double y = getGridY( j );
            for ( unsigned int i = 0 ; i < nx_ ; i++, cc++) // loop over x
            {
                double x = getGridX( i );
                size_t offset = 3*cc;
                coords[offset + 0 ] = x;//getGridX( i );
                coords[offset + 1 ] = y;//getGridY( j );
                coords[offset + 2 ] = z;//getGridY( k );
            }
        }// end loop over y
    }// end loop over z

// GENERATE INDEX TO TETS THAT CONTAIN EARCH REGULAR GRID POINT
// INDEX WILL HAVE SPECIAL VALUE Geom::NOT_AN_INDEX, IF POITN WAS NOT FOUND
// THIS MAY HAPPEN WHEN THE UNDERLYING TET MESH IS NOT A QUBE
    std::vector< unsigned int > indT; // index to tet containing a regular coordinate
    geom.genIndToTetsByCoords(indT,
                              coords,
                              npr_,
                              false); // do NOT terminate app if a coord is not found


// NOW CALCULATE WEIGHTS AND NODE INDEXES FOR EACH REGULAR GRID POINT
    lookupList.clear();
    lookupList.reserve( npr_ );
    Mesh* t = geom.t;   // TETRAHEDRAL MESH. OBVIOUSLY THIS IS EVIL...
    double* p = geom.getPtrTop();
    for (unsigned int i = 0; i < npr_ ; i++)
    {

        lookup lu;                  // NEW LOOKUP TABLE ENTRY
        lu.type = RegularGrid::OK;  // INITIALISE TO GOOD

        if ( indT[i] != Geometry::NOT_AN_INDEX ) // IF CONTAINING TET ELEMENT WAS FOUND
        {
            double* coord = &coords[3*i]; // pointer to coordinates of i'th regular point
            // SET INDEXES TO NEIGHBOURING VERTEXES
            lu.ind[0] = t->getNode( indT[i], 0 );
            lu.ind[1] = t->getNode( indT[i], 1 );
            lu.ind[2] = t->getNode( indT[i], 2 );
            lu.ind[3] = t->getNode( indT[i], 3 );

            // CALCULATE NEIGHBOUR NODE WEIGHTS - THESE ARE THE LOCAL COORDINATES
            // OF THE VERTEXES OF THE TET CONTAINING THE REGULAR POINT
            t->CalcLocCoords( indT[i], p, coord, &lu.weight[0] );
        }
        else // CONTAINING TET ELEMENT WAS NOT FOUND
        {
            lu.type = RegularGrid::NOT_FOUND;   // containing element not found

            lu.ind[0] = NOT_AN_INDEX; lu.ind[1] = NOT_AN_INDEX;
            lu.ind[2] = NOT_AN_INDEX; lu.ind[3] = NOT_AN_INDEX;
            lu.weight[0] = 0; lu.weight[1] = 0;
            lu.weight[2] = 0; lu.weight[3] = 0;
        }
        lookupList.push_back( lu );
    }


    delete [] coords;
    return true;


}

void RegularGrid::interpolateToRegular(const double*& sclrIn,
                                       double*& sclrOut)
{
    // INTERPOLATES SINGLE SCALAR FROM TET MESH TO REGULAR GRID
    if (!npr_)
    {

    }

    for ( size_t i = 0 ; i < lookupList.size() ; i ++)
    {
        lookup L = lookupList[i];

        if (L.type == RegularGrid::OK )
        {
            double s0 = sclrIn[ L.ind[0] ];
            double s1 = sclrIn[ L.ind[1] ];
            double s2 = sclrIn[ L.ind[2] ];
            double s3 = sclrIn[ L.ind[3] ];

            sclrOut[i] = L.weight[0]*s0 + L.weight[1]*s1 + L.weight[2]*s2 + L.weight[3]*s3;
        }
        // IF NON-LC LOCATION - FIX THIS
        else
        {
            sclrOut[i] = -1;
        }
    }

}

double RegularGrid::interpolateNode(const double* valuesIn,
                       const RegularGrid::lookup& L)const
{

    return valuesIn[ L.ind[0] ]*L.weight[0] +
            valuesIn[L.ind[1] ]*L.weight[1] +
            valuesIn[L.ind[2] ]*L.weight[2] +
            valuesIn[L.ind[3] ]*L.weight[3];


}

void RegularGrid::interpolateToRegular(const double *&vecIn,
                                       double *&vecOut,
                                       const size_t &np)

{
    // INTERPOLATES A x,y,z-VECTOR TO REGULAR GRID
    if (!npr_)
    {
        printf("error in %s, Regular grid doesn't seem to be initialised - bye!\n", __func__);
        exit(1);
    }

    for ( size_t i = 0 ; i < lookupList.size() ; i++ )
    {
        lookup L = lookupList[i];
        if (L.type == RegularGrid::OK )
        {
            double nx = interpolateNode( vecIn, L );
            double ny = interpolateNode( vecIn+np , L );
            double nz = interpolateNode( vecIn+2*np, L);

            vecOut[i+0*npr_] = nx;
            vecOut[i+1*npr_] = ny;
            vecOut[i+2*npr_] = nz;
        }
        // IF TRYING TO INTEPOLATE DIRECTOR TO A NON-LC GRID NODE
        else if (L.type == RegularGrid::NOT_LC )
        {
            vecOut[i + 0 * npr_] = 0;
            vecOut[i + 1 * npr_] = 0;
            vecOut[i + 2 * npr_] = 0;
        }
    }
}


bool RegularGrid::writeVTKGrid(const char *filename,
                               const double *pot,
                               const double *n,
                               const size_t& npLC)
{
// WRITES POTENTIAL, ORDER PARAMETER AND DIRECTOR ONTO VTK REGULAR GRID FILE

    if (npr_ == 0 )
    {
        printf("error in %s, regular grid doesn't seem to be initialised - bye!\n", __func__);
        exit(1);
    }

    std::fstream fid;
    fid.open( filename, std::fstream::out );

    if (!fid.is_open() )    // EXIT IF COULDN'T OPEN FILE
        return false;


    double* regU = new double[ npr_ ];  // TEMPORARY STORAGE FOR INTERPOLATED VALUES
    double* regN = new double[ 3*npr_];
    double* regS = new double[ npr_];

    const double* S = n+3*npLC;   // START OF IRREGULAR S

    interpolateToRegular( pot , regU );     // IRREGULAR TO REGULAR CONVERSION
    interpolateToRegular( n , regN, npLC );
    interpolateToRegular( S , regS);

    int num_points[3] = {nx_, ny_, nz_};
    double grid_spacing[3] = {dx_, dy_, dz_};

    bool ret = true;        // RETURN VALUE

    vtkIOFun::writeID( fid );

    vtkIOFun::writeHeader( fid,
                           "header string",
                           vtkIOFun::ASCII,
                           num_points,
                           grid_spacing);

    vtkIOFun::writeScalarData( fid , npr_, "potential", regU);
    vtkIOFun::writeScalarData( fid , npr_, "S", regS);
    vtkIOFun::writeVectorData( fid, npr_, "director", regS);

    delete [] regU;
    delete [] regN;
    delete [] regS;

    return true;

}

>>>>>>> 5385d0f5615cb5b4d1e7b2fb7506a9397ef2a0db
