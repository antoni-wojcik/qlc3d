#include <stdio.h>
#include <stdlib.h>
#include <qlc3d.h>
#include <string>
#include <vector>
#include <reader.h>
#include <iostream>
#include <meshrefinement.h>
#include <filesysfun.h>
using namespace std;


void problem(const char *error)
{
    /*! Prints warning message for a bad setting and exits*/
	printf("WARNING - problem reading setting for : ");
	printf("%s\n",error);
	printf("\n");
	exit(1);
}
void problem(std::string& name, int ret){
    /*! Checks reader return value. If it is an error, prints error message and quits.
        Use this for mandatory settings.
     */
    Reader temp;
    if (ret!= READER_SUCCESS){
        cout << "Problem reading " << name << " , computer says: " << temp.getErrorString(ret) << endl;
        cout << "bye!"<<endl;
        exit(1);
    }
}
void problemo(std::string& name, int ret){
   /*! Checks reader return value. If it is an error, prints warning message but does not quit.
       Use this for optional settings.*/
    Reader temp;
	if ( (ret!= READER_SUCCESS) ){
        cout << "Problem reading " << name << " , computer says: " << temp.getErrorString(ret) << endl;

    }
}
// end problemo (optional parameter problem)


void readLC(LC& lc,Reader& reader)
{

    /// THIS CHECKING SHOULD BE DONE IN THE READER CLASS
//	reader.file.seekg(0);
//	reader.file.clear();
	if ( ! reader.file.good() ){
	cout << "error reading LC - bye!" <<endl;
	exit(1);
    }
	string name = "";
	int ret     = 0;
	double val  = 0;
// ELASTIC COEFFS
	name = "K11";
	ret = reader.readNumber(name , val);
	if (ret == READER_SUCCESS)
        lc.K11 = val;
	problem(name, ret);

	name = "K22";
	ret = reader.readNumber(name , val);
	if (ret == READER_SUCCESS)
        lc.K22 = val;
    problem(name, ret);

	name = "K33";
	ret = reader.readNumber(name , val);
	if (ret == READER_SUCCESS)
        lc.K33 = val;
    problem(name, ret);

	name = "p0";
	ret = reader.readNumber(name , val);
	if(ret == READER_SUCCESS)
        lc.p0 = val;
    problem(name, ret);
// THERMOTROPIC COEFFS
	name = "A";
	ret = reader.readNumber(name , val);
	if(ret == READER_SUCCESS)
        lc.A = val;
    problem(name,ret);

	name = "B";
	ret = reader.readNumber(name, val);
	if(ret == READER_SUCCESS)
        lc.B = val;
    problem(name,ret);

	name = "C";
	ret = reader.readNumber(name, val);
	if(ret == READER_SUCCESS)
        lc.C = val;
    problem(name,ret);

// ELECTRIC COEFFS
	name = "eps_par";
	ret = reader.readNumber(name, val);
	if(ret == READER_SUCCESS)
        lc.eps_par = val;
    problem(name, ret);

	name = "eps_per";
	ret = reader.readNumber(name, val);
	if(ret == READER_SUCCESS)
        lc.eps_per = val;
    problem(name, ret);

	name = "e11";
	ret = reader.readNumber(name, val);
	if(ret == READER_SUCCESS)
        lc.e11 = val;
    problem(name,ret);

	name = "e33";
	ret = reader.readNumber(name, val);
	if(ret == READER_SUCCESS)
        lc.e33 = val;
    problem(name, ret);

// VISCOUS COEFFICIENTS
    name = "gamma1";
    ret = reader.readNumber(name, val);
    if(ret == READER_SUCCESS)
        lc.gamma1 = val;
	problem(name, ret);

    name = "gamma2";
    ret = reader.readNumber(name, val);
    if(ret == READER_SUCCESS)
        lc.gamma2 = val;
	problem(name, ret);

// DO THIS LATER IF NEEDED
   // name = "alpha1";
   // name = "aplha2";
   // name = "alpha4";
   // name = "alpha5";
   // name = "alpha6";

}//end void readLC


void readSimu(Simu* simu, Reader& reader, EventList& evel)
{
	std::string name;
	std::string str_var;
	int         int_var = 0;
	double      dbl_var = 0;
	int         ret;

//  READ STRING SETTINGS
//  MANDATORY STRING SETTINGS

    ret = 0;
    name = "EndCriterion";
    ret = reader.readString(name , str_var);
    
	
    if ( ret== READER_SUCCESS)
        simu->setEndCriterion(str_var);

    problem(name, ret);

    name = "MeshName";
    ret = reader.readString(name, str_var);
    if ( ret == READER_SUCCESS )
        simu->MeshName = str_var;

    problem(name, ret);

// OPTIONAL STRING SETTINGS
    name = "LoadQ";
    ret = reader.readString(name , str_var);
    if (ret == READER_SUCCESS)
        simu->setLoadQ(str_var);
    //problemo(name, ret);

    name = "SaveDir";
    ret = reader.readString(name , str_var);
    if ( ret == READER_SUCCESS) // if specified, use it
    {
        simu->setSaveDir( simu->getCurrentDir() + "/" + str_var);
    }
    else // otherwise use default ( = res )
    {
        simu->setSaveDir( simu->getCurrentDir() + "/" + simu->getSaveDir() );
    }
    //problemo(name , ret);
//========================
// SCALAR VALUES
//========================
    name = "SaveIter";
    ret = reader.readNumber(name, int_var);
    if (ret == READER_SUCCESS)
    {
        simu->setSaveIter(int_var);
        evel.setSaveIter( (size_t) int_var );
    }
    //problemo(name, ret);
    name = "EndValue";
    ret = reader.readNumber(name, dbl_var);
    if (ret == READER_SUCCESS)
        simu->setEndValue(dbl_var);
    problem(name, ret);

    name = "dt";
    ret = reader.readNumber(name , dbl_var);
    if (ret == READER_SUCCESS)
        simu->setdt(dbl_var);
//    problemo(name, ret);

    name = "TargetdQ";
    ret = reader.readNumber(name , dbl_var);
    if (ret == READER_SUCCESS)
	simu->setTargetdQ(dbl_var);
    else{
	exit(1);
    }
    //problemo(name, ret);

    name = "Maxdt";
    ret = reader.readNumber(name , dbl_var);
    if(ret==READER_SUCCESS)
        simu->setMaxdt(dbl_var);
    //problemo(name, ret);

    name = "MaxError";
    ret = reader.readNumber(name, dbl_var);
    if(ret==READER_SUCCESS)
        simu->setMaxError(dbl_var);
    //problemo(name, ret);

    name = "OutputEnergy";
    ret  = reader.readNumber(name , int_var);
    if ( ret == READER_SUCCESS)
        simu->setOutputEnergy(int_var);
    //problemo(name, ret);

    name = "OutputFormat";
    ret = reader.readNumber(name , int_var);
    if(ret == READER_SUCCESS)
        simu->setOutputFormat(int_var);

    name = "SaveFormat";
    std::vector < std::string > vec_str;
    ret = reader.readStringArray( name , vec_str );

    if ( ret == READER_SUCCESS )
    {
        if (vec_str.size() == 0 )
        {
            simu->addSaveFormat( Simu::SF_LCVIEW );
        }
        else
        {
            for (size_t i = 0 ; i < vec_str.size() ; i++)
            {
                simu->addSaveFormat( vec_str[i] );
            }
        }

    }
    else if (ret == READER_NOT_FOUND)   // IF NOT SPECIFIED, BY DEFAULT USE LCview
    {
        simu->addSaveFormat( Simu::SF_LCVIEW );
    }
    else
        problem(name, ret);

//------------------------------------------
//
// READ VECTOR VALUES
//
//------------------------------------------
    name = "StretchVector";
    vector <double> vec;

	ret = reader.readNumberArray(name , vec );
    if (ret == READER_SUCCESS){
        if (vec.size() != 3){
            cout << "error - invalid StretchVector length - bye!\n"<<endl;
            exit(1);
        }
        //cout << "stretch :"<< vec[0] <<","<< vec[1] <<"," << vec[2] << endl;
        simu->setStretchVectorX(vec[0]);
        simu->setStretchVectorY(vec[1]);
	simu->setStretchVectorZ(vec[2]);
        //cout << "stretch" << simu->getStretchVectorX() << "," << simu->getStretchVectorY() << "," << simu->getStretchVectorZ() << endl;
    }
    //problemo(name, ret);

	name = "EnergyRegion";
	ret = reader.readNumberArray(name , vec);

	if ((ret == READER_SUCCESS) && (vec.size() == 3)){

		simu->setEnergyRegionX(vec[0]);
		simu->setEnergyRegionY(vec[1]);
		simu->setEnergyRegionZ(vec[2]);
	}
        //problemo(name , ret);

	name = "dtLimits";
	ret = reader.readNumberArray(name , vec);
	if ( (ret == READER_SUCCESS) && (vec.size() == 2)){
	    simu->setdtLimits(vec[0], vec[1]);
	}
        //problemo (name , ret );


	name = "dtFunction";
	ret = reader.readNumberArray(name , vec );
	if ((ret == READER_SUCCESS) && (vec.size() == 4) ){
	    double F[4] ={vec[0] , vec[1] , vec[2], vec[3]};
	    simu->setdtFunction(F);
	}
        //problemo(name , ret);



//------------------------------------------
//
// READ POTENTIAL CONSISTENCY CALCULATION
//
//------------------------------------------

// DETERMINES WHICH METHOD IS USED FOR POTENTIAL CONSISTENCY
	//name = "PotCons";
	//ret = reader.readString(name, str_var);
	//if (ret == READER_SUCCESS){
	//	if ( str_var.compare("Off") == 0 )
	//		simu->setPotCons(Off);
	//	else if (str_var.compare("Loop") == 0)
	//		simu->setPotCons(Loop);
	//}
	//problemo("PotCons");
// SETS MINIMUM CONSISTENCY ACCURACY
	//name = "TargetPotCons";
	//ret = reader.readNumber(name, dbl_var);
	//if (ret == READER_SUCCESS){//
	//	simu->setTargetPotCons(dbl_var);
	//}
	//
//else
	//
	//problemo("TargetPotCons");

}//end void readSimu

void readBoxes(Boxes* boxes, Reader& reader) //config_t* cfg)
{
	int maxbox = 100; // maximum number of supported boxes...
	for (int i = 1 ; i < maxbox ; i++){ // for boxes 1-99
	   // cout << i << endl;
	    stringstream box;
	    string name;
	    string str_val;

	    int ret;

            box << "BOX" << i <<".Type";
	    box >> name;


		reader.file.seekg(0);
		reader.file.clear();
		if (!reader.file.good()){
            cout << "error reading Boxes, file pointer is bad - bye!" << endl;
            exit(1);
        }

	    ret = reader.readString(name , str_val);
        //=============================
        // ADD BOXES IF THEY EXIST
        //=============================
        if (ret == READER_SUCCESS){ // if Boxi exists
           // cout << "reading box " << i << endl;
            Box* b = new Box();     // CREATE EMPTY BOX OBJECT
            b->Type = str_val;

            vector <double> par;


            box.clear();
            name.clear();
            box << "BOX" << i <<".Params";
            box >> name;


            ret = reader.readNumberArray(name , par);
            problem(name, ret);
            b->setParams(par);

    // BOX X
            box.clear();
            name.clear();
            box << "BOX" << i <<".X";
            box >> name;
            ret = reader.readNumberArray(name , par);

	    //cout << "X :" << par[0] << "," << par[1] << endl;
	    problem(name, ret);
            b->setX(par);


	// BOX Y
            box.clear();
            name.clear();
            box << "BOX" << i <<".Y";
            box >> name;
            ret = reader.readNumberArray(name, par);
            problem(name, ret);
            b->setY(par);
	// BOX Z
            box.clear();
            name.clear();
            box << "BOX" << i <<".Z";
            box >> name;
            ret = reader.readNumberArray(name , par);
            problem(name , ret);
            b->setZ(par);
	// BOX TILT
            box.clear();
            name.clear();
            box << "BOX" << i <<".Tilt";
            box >> name;
            ret = reader.readNumberArray(name, par);
	    problem(name , ret);
            b->setTilt(par);
	// BOX TWIST
            box.clear();
            name.clear();
            box << "BOX" << i <<".Twist";
            box >> name;
            ret = reader.readNumberArray(name, par);
            problem(name, ret);
            b->setTwist(par);

	// ADD TO LIST OF BOXES
            boxes->addBox(b);

        }// end if box exists
	}
    //end for i = 1:maxbox

}// end void readBoxes

void readAlignment(Alignment* alignment, Reader& reader)
{

	int num_surfaces = 99; // limit surfaces to 0-99
	std::string surface_name;
	std::string surface_setting_name;


	for ( int i = 0; i < num_surfaces; i++)	{
        stringstream ss;
        string name;
        string str_val;
        int ret = 0;
        ss << "FIXLC" << i << ".Anchoring";
        ss >> name;

        ret = reader.readString(name , str_val);

        if ( ret == READER_SUCCESS){
            Surface* s = new Surface();
            s->setAnchoringType(str_val);

            double dbl_val;

            ss.clear(); name.clear();
            ss << "FIXLC" << i << ".Strength";
            ss >> name;
            ret = reader.readNumber(name , dbl_val);
            if ( ! s->getisFixed() ) {          // if not a fixed surface, set  mandatory strength
                problem(name , ret);
                s->setStrength(dbl_val);
            }
            else
                s->setStrength(1e99);


            ss.clear(); name.clear();
            ss << "FIXLC"<<i<<".Easy";
            ss >> name;
            vector < double > vec;
            ret = reader.readNumberArray( name , vec );
            problem( name , ret );
            if (vec.size() == 3 ){
                double ez[3] = {vec[0] , vec[1] , vec[2] };
                s->setEasyAngles( ez );
                s->calcV1V2(); // calculates surface vectors from easy angles
            }
            else{
                cout << "error - Easy direction "<< name <<" needs 3 angles - bye!" << endl;
                exit(1);
            }

            ss.clear(); name.clear();
            ss << "FIXLC" << i << ".K1";
            ss >> name;
            ret = reader.readNumber( name , dbl_val);
            if (! s->getisFixed () ){
                problem(name , ret);
                s->setK1(dbl_val);
            }
            ss.clear(); name.clear();
            ss << "FIXLC" << i << ".K2";
            ss >> name;
            ret = reader.readNumber( name , dbl_val);
            if (! s->getisFixed () ){
                problem(name , ret);
                s->setK2(dbl_val);
            }


            alignment->addSurface(s);
        }
        // end if FXLCi exists
    }// end for every FIXLC#

}//end void readAlignment

void readElectrodes(Electrodes* electrodes,
                    Reader& reader,
                    EventList& evli)
{

    std::string name;
    std::stringstream ss;
    // COUNT NUMBER OF ELECTRODES
    int i = 1;
    for ( ; i < 100 ; i++)
    {

        std::vector <double> dummy;

        name.clear();
        ss.clear();
        ss << "E"<< i <<".Pot";
        ss >> name;

        int ret = reader.readNumberArray(name,dummy );

        if (ret!=READER_SUCCESS)
            break;
    }
    size_t numElectrodes = i - 1 ;
    electrodes->setnElectrodes( numElectrodes );



    for (size_t i = 1 ; i < numElectrodes + 1 ; i++)
    {
        std::vector<double> times;
        std::vector<double> pots;

        name.clear();
        ss.clear();
        ss << "E"<<i<<".Time";
        ss >> name;
        int ret = reader.readNumberArray(name,times);
        problem(name, ret);

        name.clear();
        ss.clear();
        ss << "E"<<i<<".Pot";
        ss >> name;
        ret = reader.readNumberArray(name, pots);

        if (times.size() != pots.size() )
        {
            cout <<" error reading E" << i <<" Time/Pot lengths do not match - bye!"<<endl;
            exit(1);
        }


        // ADD CODE HERE TO DISTINGUISH BETWEEN TIME/ITERATION SWITCHING
        for (size_t j = 0 ; j < times.size() ; j++)
        {
            SwitchingEvent *swEvent = new SwitchingEvent( times[j], pots[j] , i-1 ); // CREATE NEW SWITCHING EVENT ON HEAP
            evli.insertTimeEvent( swEvent );
        }
    }

    evli.printEventList();

}
// end readElectrodes

void readMeshrefinement( MeshRefinement* meshrefinement, Reader& reader){
    int num_refreg = 100;
    stringstream ss;
    string name = "";
    std::vector < double > vec;

    std::string str_val;
    int ret = 0;

    // READS REFREG STRUCTURES
    for ( int i = 0 ; i < num_refreg ; i++){
	ss.clear(); name.clear();
	ss << "REFREG" << i << ".Type";
	ss >> name;

	ret = reader.readString(name , str_val);

        if (ret == READER_SUCCESS ){ // if this REFREG was found
	    cout << " reading REFREG" << i << endl;
	    RefReg rr;
	    rr.setType(str_val);

	    // READ REGION COORDINATES
	    // READ X
	    ss.clear(); name.clear();
	    ss << "REFREG" << i <<".X";
	    ss >> name;
	    ret = reader.readNumberArray(name, vec);
	    problem(name, ret);
	    rr.addX( vec );
	    // READ Y
	    ss.clear(); name.clear();
	    ss << "REFREG" << i <<".Y";
	    ss >> name;
	    ret = reader.readNumberArray(name, vec);
	    problem(name, ret);
	    rr.addY( vec );
	    // READ Z
	    ss.clear(); name.clear();
	    ss << "REFREG" << i <<".Z";
	    ss >> name;
	    ret = reader.readNumberArray(name, vec);
	    problem(name, ret);
	    rr.addZ( vec );
	    // READ DISTANCE
	    ss.clear(); name.clear();
	    ss << "REFREG" << i <<".Distance";
	    ss >> name;
	    ret = reader.readNumberArray(name, vec);
	    cout << name << " " << ret << "length vec = " << vec.size() << endl;
	    problem(name, ret);
	    rr.addDistance( vec );
	    //rr.PrintRefinementRegion();
	    meshrefinement->addRefinementRegion( rr );
	}
    }
}//end readMeshrefinement
void readAutorefinement( MeshRefinement* meshrefinement, Reader& reader){
    int num_autoref = 100;
    stringstream ss;
    string name = "";
    std::vector <double> vec_val;
    std::string str_val;
    double dbl_val;
    int ret = 0;
    for (int i = 0 ; i < num_autoref; i++){ // fro autorefs, i
        ss.clear();
        name.clear();

        ss <<"AUTOREF"<< i << ".Type";
        ss >> name;

        ret = reader.readString(name, str_val);

        if (ret == READER_SUCCESS){ // IF THIS AUTOREFINEMENT IS FOUND
            cout << "reading AUTOREF"<< i << endl;
            AutoRef ar;
            if(!ar.setType(str_val)){
                cout << "error - invalid AUTOREF"<<i << " Type: " << str_val <<" - bye!" << endl;
                exit(1);
            }

            ss.clear(); name.clear();
            ss << "AUTOREF" << i <<".RefIter";
            ss >> name;
            ret = reader.readNumber(name, dbl_val );
            problem(name, ret);
            ar.setRefIter((int) dbl_val );

            // READ MaxValue
            ss.clear(); name.clear();
            ss << "AUTOREF" << i <<".MaxValue";
            ss >> name;
            ret = reader.readNumberArray( name, vec_val );
            problemo(name, ret );
            ar.addMaxValue( vec_val );

            // READ MinSize
            ss.clear(); name.clear();
            ss << "AUTOREF"<<i<<".MinSize";
            ss >> name;
            ret = reader.readNumber( name , dbl_val);
            problemo ( name, ret );
            ar.setMinSize(dbl_val);
            printf("Autoref.MinSize = %e\n", dbl_val);
            meshrefinement->addAutorefinement( ar );

        }// end if autoref i found
    }// end for possible autorefs, i

}

void readEndrefinement( MeshRefinement* meshrefinement, Reader& reader){
    int num_endref = 10;
    stringstream ss;
    string name = "";
    std::vector <double> vec_val;
    std::string str_val;
    double dbl_val;
    int ret = 0;
    for (int i = 0 ; i < num_endref; i++){ // fro autorefs, i
        ss.clear();
        name.clear();

        ss <<"ENDREF"<< i << ".Type";
        ss >> name;
        //cout << name << endl;
        ret = reader.readString(name , str_val);
        //printf(" ret = %i\n", ret );
        if (ret == READER_SUCCESS){ // IF THIS AUTOREFINEMENT IS FOUND
            cout << "reading ENDREF"<< i << endl;
            EndRef er;
            if(!er.setType(str_val)){
                cout << "error - invalid ENDREF"<<i << " Type: " << str_val <<" - bye!" << endl;
                exit(1);
            }

            ss.clear(); name.clear();
            ss << "ENDREF" << i <<".RefIter";
            ss >> name;
            ret = reader.readNumber(name, dbl_val );
            problem(name, ret);
            er.setRefIter((int) dbl_val );

            // READ MaxValue
            ss.clear(); name.clear();
            ss << "ENDREF" << i <<".MaxValue";
            ss >> name;
            ret = reader.readNumberArray( name, vec_val );
            problemo(name, ret );
            er.addMaxValue( vec_val );

            // READ MinSize
            ss.clear(); name.clear();
            ss << "ENDREF"<<i<<".MinSize";
            ss >> name;
            ret = reader.readNumber( name , dbl_val);
            problemo ( name, ret );
            er.setMinSize(dbl_val);
            printf("Endref.MinSize = %e\n", dbl_val);
            meshrefinement->addEndrefinement( er );

        }// end if autoref i found
    }// end for possible autorefs, i
}


void ReadSettings(
	string settings_filename,
	Simu* simu,
        LC& lc,
	Boxes* boxes,
	Alignment* alignment,
	Electrodes* electrodes,
        MeshRefinement* meshrefinement,
        EventList& eventlist)
{

    Reader reader;
    using namespace std;
    if ( reader.openFile(settings_filename) ){
        cout << "reading: "<< settings_filename << endl;
    // READ SIMU
    readSimu(simu , reader, eventlist);

    // READ LC
    readLC(lc , reader);
    lc.convert_params_n2Q();

    // READ BOXES
    readBoxes(boxes , reader);

    // READ ALIGNMENT SURFACES
    readAlignment(alignment, reader);

    // READ ELECTRODES
    readElectrodes(electrodes , reader, eventlist);
    // READ MESH REFINEMENT
    readMeshrefinement(meshrefinement, reader);
    // READ AUTOREFINEMENT
    readAutorefinement(meshrefinement, reader);

    // READ ENDREFINEMENT
    readEndrefinement(meshrefinement, reader);


    reader.closeFile();
    }
    else{
        cout << "error - could not open " << settings_filename <<" - bye!" << endl;
        exit(1);
    }
}
// end ReadSettings



void ReadSolverSettings(const char* filename, Settings* settings)
{
    // READS SOLVER SETTINGS FROM FILE. THIS WAS ORIGIANLLY ASSUMED TO BE
    // CALLED "solver.qfg", BUT NOW ALSO READS "solver.txt", IF "solver.qfg"
    // IS NOT FOUND
    std::string fn = filename;
    if ( !FilesysFun::fileExists(fn) )
    {
        fn = "solver.txt";
    }


    Reader reader;
    if ( reader.openFile(fn) )
    {

        int i_val;
        double dbl;
        int ret;
        string name;

        name = "nThreads";
        ret = reader.readNumber(name, i_val);
        problem(name, ret);
        settings->setnThreads(i_val);

        name = "Q_Solver";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_Solver( i_val);

        name = "V_Solver";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setV_Solver( i_val);

        name = "Q_Newton_Panic_Iter";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_Newton_Panic_Iter( i_val);

        name = "Q_Newton_Panic_Coeff";
        problem(name , reader.readNumber(name , dbl ) );
        settings->setQ_Newton_Panic_Coeff(dbl);


        name = "Q_PCG_Preconditioner";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_PCG_Preconditioner( i_val);

        name = "Q_PCG_Maxiter";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_PCG_Maxiter( i_val);


        name = "Q_PCG_Toler";
        problem(name , reader.readNumber(name , dbl ) );
        settings->setQ_PCG_Toler(dbl);

        name = "Q_GMRES_Preconditioner";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_GMRES_Preconditioner(i_val);


        name = "Q_GMRES_Maxiter";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_GMRES_Maxiter( i_val);


        name = "Q_GMRES_Restart";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setQ_GMRES_Restart(i_val);

        name = "Q_GMRES_Toler";
        problem(name , reader.readNumber(name , dbl) );
        settings->setQ_GMRES_Toler(dbl);

        name = "V_PCG_Preconditioner";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setV_PCG_Preconditioner( i_val);

        name = "V_PCG_Maxiter";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setV_PCG_Maxiter(i_val);

        name = "V_PCG_Toler";
        problem(name , reader.readNumber(name , dbl ) );
        settings->setV_GMRES_Toler(dbl);

        name = "V_GMRES_Preconditioner";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setV_GMRES_Preconditioner( i_val);

        name = "V_GMRES_Maxiter";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setV_GMRES_Maxiter( i_val);

        name = "V_GMRES_Restart";
        problem(name , reader.readNumber(name , i_val ) );
        settings->setV_GMRES_Restart( i_val);

        name = "V_GMRES_Toler";
        problem(name , reader.readNumber(name , dbl ) );
        settings->setV_GMRES_Toler( dbl );

        //settings->PrintSettings();

        reader.closeFile();

    }
    else
    {
        cout<< "error - could not read solver settings file:" << fn << " - bye!"<< endl;
        exit(1);
    }

} // end readSolverSettings

