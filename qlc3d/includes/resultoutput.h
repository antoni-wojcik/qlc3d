#ifndef RESULTOUTPUT_H
#define RESULTOUTPUT_H


#include <vtkiofun.h>
#include <regulargrid.h>
#include <simu.h>
#include <lc.h>
#include <geometry.h>
#include <solutionvector.h>
#include <meshrefinement.h>


namespace LCviewIO
{

static const char LCVIEW_TEXT_FORMAT_STRING[] = "%i %f %f %f %f %f %f\n";
// FOLLOWING FUNCTIONS ARE DEFINED IN WriteLCD.cpp

/*
void WriteLCViewResult(Simu* simu, 		// Simulation settings
                LC* lc,				// LC material paramters
                Geometry* geom,		// mesh geometry data
                SolutionVector* v,  // potential solution
                SolutionVector* q); // Q-tensor solution
                //MeshRefinement* meshref = NULL); // meshrefinement info. including whether a new mesh has been generated
*/
// WRITES LCVIEW RESULT IN TEXT FORMAT
void WriteLCD(double *p, Mesh *t, Mesh *e, SolutionVector *v, SolutionVector *q, Simu* simu, int currentIteration, double currentTime);
// WRITES LCVIEW RESULT IN BINARY FORMAT
void WriteLCD_B(double *p,
                Mesh *t, Mesh *e,
                SolutionVector *v, SolutionVector *q,
                int currentIteration,
                double currentTime,
                Simu *simu, LC* lc);
void ReadResult(Simu& simu,         // READS AND LOADS Q-TENSOR VALUES FROM AN EXISTING RESULT FILE
                SolutionVector& q); // TRIES TO FIGURE OUT WHETHER RESULT FILE IS IN TEXT OR BINARY FORMAT




void CreateSaveDir(Simu& simu);
void ReadLCD_B(Simu* simu, SolutionVector* q);
void ReadLCD_T(Simu& simu, SolutionVector& q); // LOADS TEXT FORMAT LCVIEW RESULT FILE
// REPLACE WRITESETTNGS WITH DIRECT COPY OF
// SETTINGS FILE
//
//void WriteSettings(Simu* simu,
//                   LC* lc,
//                   Boxes* box,
//                   Alignment* alignment ,
//                   Electrodes* electrodes);
}
#endif // RESULTOUTPUT_H
