//#include "qlc3d.h"
#include <stdio.h>
#include "sparsematrix.h"
#include "mesh.h"
#include <globals.h>
struct spm{				// define sparce matrix link
    idx row;
    spm * next;
    spm * prev;
};
void sp_elem(spm *col, idx r, idx c, idx *nnz);

void sparsecreatell(Mesh *mesh, SparseMatrix *K)//double **P, int **I, int **J , int *nnz)
{

    idx maxp	= -10;// number of columns

    idx i=0,n1=0,n2=0,x=0; // counters & temporary indicies
    //idx *tt;		// temporary element

    idx dof_per_node =1; // assume this is same for all
    // determine matrix size -find largest node number
    idx nnz[1]={0}; // this is stupid, but I am too lazy ...





    // FIND LARGEST NODE NUMBER IN INPUT MESH
    idx nNodes = mesh->getnNodes(); // nodes per element
    for (idx element = 0; element < mesh->getnElements(); element++){ //loop number of elements
        for(idx node = 0; node < nNodes ; node ++){ //loop nodes per element
            if (mesh->getNode(element,node) > maxp){
                maxp = mesh->getNode(element,node);
            }
        }
    }

    maxp++;
    spm *col = new spm[maxp];	// allocate column of pointers

    for (i=0;i<maxp;i++){
        col[i].next = NULL;  // initialize with no elements
        col[i].prev = NULL;
        col[i].row  = 0;
    }

    // fill in first row of blocks... column blocks are added later
    idx r,c;
    idx *tt = (idx*)malloc(dof_per_node * sizeof(idx));

    for (idx el = 0; el< mesh->getnElements() ; el ++ ){
        for (x=0;x<dof_per_node;x++) // this loop is redundant in potential calculation
        {
            for (n1=0 ; n1< mesh->getnNodes() ; n1++) // loop over each node in element "i" first loop
            {

                r = mesh->getNode(el,n1) + x*maxp; //sparse row index

                for (n2=n1+1 ; n2< mesh->getnNodes(); n2++)// loop over each node in element "i" second loop
                {
                    c = mesh->getNode(el,n2) + x*maxp;		// column index

                    sp_elem(col, mesh->getNode(el,n1), r, nnz); // add row,col n1,n2 nodes to LL
                    sp_elem(col, mesh->getNode(el,n1), c, nnz);
                    sp_elem(col, mesh->getNode(el,n2), r, nnz);
                    sp_elem(col, mesh->getNode(el,n2), c, nnz);

                }//end for n2
            }//end for n1
        }//end  for x
    }//end for el

    // ALLOCATE SPARSE MATRIX ARRAYS
    //printf("nnz= %i\n",nnz[0]);
    //double *Pr = (double*)malloc(nnz[0]*sizeof(double));
    int    *Ir = (int*)malloc(nnz[0]*sizeof(int));
    int    *Jc = (int*)malloc((maxp+1)*sizeof(int));
    //memset(Pr,0,nnz[0]*sizeof(double));
    memset(Ir,0,nnz[0]*sizeof(int));
    memset(Jc,0,(maxp+1)*sizeof(int));

    // FILL IN CONNECTIVITY INFORMATION TO Ir AND Jc ARRAYS FROM LINKED LISTS
    // Linked list is freed as it is copied into arrays
    spm *node;
    spm *temp;
    int ir = 0;
    int jc = 0;
    nnz[0]=0; // counts number of entries

    for (i=0;i<maxp*dof_per_node;i++)// loop through col[i];
    {
        node= &col[i];
        node = node->next; // first node in list
        Jc[jc]=nnz[0];		   // column index
        while (node!=NULL) // follow list untill NULL
        {
            Ir[ir] = (node->row) ;	     // set row number in Ir
            nnz[0]++;
            ir++; //next position in Ir array

            //Cleanup, free linked list on the go...
            temp=node;
            node = node->next;
            if (!temp) delete temp;//free(temp);

        }//end while

        if (!node) delete node;//free(node);		  // free linked list, so this doesnt need to be done later
        jc++; // next position in Jc array
    }// end for i
    Jc[jc]=nnz[0]; // last value in Jc is nnz
    delete[] col;


    K->MakeSparseMatrix(maxp,maxp,nnz[0],Ir,Jc);

}//end void sprsecreatell


// void sp_elem inserts nodes into linked lists acording to row and column indicies
void sp_elem(spm *col, idx r, idx c, idx *nnz)
{
    //printf("[%i,%i] -> ",r,c);
    spm *node = &col[c]; // was &col[c-1]


    if (node->next == NULL)//first link in list
    {
        //printf("first link, column %i\n",c);
        node->next = new spm;//(spm*)malloc(sizeof(spm));
        node->next->prev = node;
        node->next->row = r;
        node->next->next = NULL;
        node=node->next;
        nnz[0] ++;
    }

    else
    {
	node = node->next; // new db
	while ((node->row <r) && (node->next!=NULL))
	{
            node = node->next;
	}

	if (node->row==r) //existing entry nothing to be done
	{
            //do nothing
            //printf("do nothing\n",r,c);
	}
	else if (node->row>r) // must insert new node in middle of list
	{
            //printf("insert in between\n",r,c);
            //spm *temp = new spm;		// new temp node
            spm *temp = new spm;//(spm*)malloc(sizeof(spm));

            node->prev->next = temp;	// insert temp node in between list << insertion fails
            temp->next=node;			//
            temp->prev=node->prev;
            node->prev=temp;			//
            temp->row = r;				// set row value
            nnz[0]++;						// add one to nonzero entries
	}

	else if(node->next==NULL) // found end of column, add new at end
	{
            //printf("add to end\n",r,c);
            //node->next = new spm;
            node->next=new spm;//(spm*)malloc(sizeof(spm));
            node->next->prev = node;	// link backwards
            node->next->next = NULL;	// link forwards = NULL, because end of list
            node->next->row = r;		// set row value
            nnz[0]++;					// add one to nonzero entries
	}
	
	else
	{printf("sparsecreatell.cpp -- !! unknown !!\n");}
    }//end else not first link*/
}

