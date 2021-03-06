/**CFile****************************************************************
 
  FileName    [threTh2Dlist.c] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [threshold.]
  
  Synopsis    [Threshold network approximation.]

  Author      [Oscar Hung]
   
  Affiliation [NTU]

  Date        [Jan 9, 2018.]

  Revision    [$Id: abc.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include "base/abc/abc.h"
#include "map/if/if.h"
#include "map/if/ifCount.h"
#include "bdd/extrab/extraBdd.h"
#include "misc/extra/extra.h"
#include "threshold.h"


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////
int    Th2DList(Thre_S * tObj);
void   Th_NtkTh2DList(Vec_Ptr_t * vThres);
void   Th_DLBuildSets(Vec_Ptr_t * vThres);
int    compare(Th_Set** i1, Th_Set** i2);

/**Function*************************************************************

  Synopsis    [Initialize 1-DL]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void   Th_NtkTh2DList(Vec_Ptr_t * vThres)
{
    int i;
    Thre_S * tObj;
    int retValue = 1;
    Vec_PtrForEachEntry(Thre_S*, vThres, tObj, i) {
        if (tObj->Type == Th_CONST1 || tObj->Type == Th_Pi) continue;
        if (!Th2DList(tObj)){
            Abc_Print(ABC_STANDARD, "Threshold gate %d is not 1-DL...\n", i);
            retValue = 0;
        }
    }
    // if (!retValue)
    //     Abc_Print(ABC_STANDARD, "Not 1-DL network...\n");
    // else
    //     Abc_Print(ABC_STANDARD, "All threshold gates are 1-DL...\n");
    Th_DLBuildSets(vThres);
    // Th_PrintSet();
    // Th_Set * t = ABC_ALLOC(Th_Set, 1);
    // int tg = 0;
    // Vec_PtrForEachEntry(Th_Set *, Golden_Vec, t, i)
    // {
    //     int k,s;
    //     Thre_S * temp = Vec_PtrEntry(vThres,t->gate);
    //     if(tg != t->gate)
    //     {
    //         printf("\n");
    //         int pp,oo;
    //         Vec_IntForEachEntry(temp->Fanins,pp,oo)
    //         {
    //             if(Vec_IntEntry(temp->FaninCs,oo)) printf("-");
    //             else printf(" ");
    //             printf("%d -> %d\n |\n v\n",pp,Vec_IntEntry(temp->dtypes,oo));
    //             if(oo == Vec_IntSize(temp->Fanins)-1) 
    //             {
    //                 if(Vec_IntEntry(temp->dtypes,oo)) printf(" 0\n");
    //                 else printf(" 1\n");
    //             }
    //         }
    //         tg = t->gate;
    //     }
    //     Vec_IntForEachEntry(t->set,s,k)
    //     {
    //         if (s > 0)
    //             Abc_Print(ABC_STANDARD, " %d ",s);
    //         else
    //             Abc_Print(ABC_STANDARD, "-%d ",-s);
    //     }
    //     Abc_Print(ABC_STANDARD, "\tgate : %d\ttype : %d\n",t->gate,t->Dtype);
    // }
}
/**Function*************************************************************

  Synopsis    [Transform a threshold gate to 1-DL]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Th2DList(Thre_S * tObj)
{
    // if(tObj->thre == 1024) printf("AND!!!!!!\n");
    int i, w;
    // int nFanins = Vec_IntSize(tObj->Fanins);
    int thre = tObj->thre;
    int sumWeight = Vec_IntSum(tObj->weights);
    int general = 0;
    int debug = 0;
    tObj->dtypes = Vec_IntStart(0);
    Vec_IntForEachEntry(tObj->weights, w, i) {
        if(general) 
        {
            Vec_IntPush(tObj->dtypes,2);
            continue;
        }
        if (sumWeight < thre) {                                                  
         Thre_S* fi = Th_GetObjById(current_TList, Vec_IntEntry(tObj->Fanins,i));
         Vec_IntPop(tObj->weights);                                            
         Vec_IntPop(tObj->Fanins);                                             
         Vec_IntPop(tObj->FaninCs);                                            
         Vec_IntRemove(fi->Fanouts, tObj->Id);                                 
         /*if (i == Vec_IntSize(tObj->weights)-1) break;                          */
         --i;                                                                  
         /*general = 1;                                                           */
         /*Vec_IntPush(tObj->dtypes, 2);                                          */
         continue;                                                             
        }                                                                        
        if (i == Vec_IntSize(tObj->weights)-1)
        {
            if (w < thre) {
                Thre_S* fi = Th_GetObjById(current_TList, Vec_IntEntry(tObj->Fanins,i));
                Vec_IntPop(tObj->weights);
                Vec_IntPop(tObj->Fanins);
                Vec_IntPop(tObj->FaninCs);
                Vec_IntRemove(fi->Fanouts, tObj->Id);
                break;
            }
            if (i > 0)
                Vec_IntPush(tObj->dtypes,Vec_IntEntryLast(tObj->dtypes));
        }
        else if (w >= thre) { // case1: 1->1
            sumWeight -= w;
            Vec_IntPush(tObj->dtypes,1);
        }
        else if (sumWeight - w < thre) { // case2: 0->0
            thre -= w;
            sumWeight -= w;
            Vec_IntPush(tObj->dtypes,0);
        }
        else 
        {
            debug = 1;
            Vec_IntPush(tObj->dtypes,2);
            if(w == Vec_IntEntry(tObj->weights,i+1))
            {
                if (2*w >= thre && sumWeight-2*w < thre) {
                    Vec_IntPush(tObj->dtypes,2);
                    thre -= w;
                    sumWeight -= 2*w;
                    i++;
                    if (w == 1) general = 1;
                }
                else general = 1;
            }
            else general = 1;
            // Vec_IntClear(tObj->dtypes);
            // return 0;
        }
    }

    // if(debug)
    // {
    //     Abc_Print(ABC_STANDARD, "Threshold gate %d is not 1-DL...\n", tObj->Id);
    //     Vec_IntForEachEntry(tObj->weights, w, i)
    //     {
    //         Abc_Print(ABC_STANDARD, "%d\t", w);
    //     }
    //     Abc_Print(ABC_STANDARD, "%d\t", tObj->thre);
    //     Abc_Print(ABC_STANDARD, "\n");
    //     Vec_IntForEachEntry(tObj->dtypes, w, i)
    //     {
    //         Abc_Print(ABC_STANDARD, "%d\t", w);
    //     }
    //     Abc_Print(ABC_STANDARD, "\n");  
    // }
    return 1;
}
/**Function*************************************************************

  Synopsis    [Find sets of 1-DL for extraction]
1000000
  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void   Th_DLBuildSets(Vec_Ptr_t * vThres)
{
    int i;
    Thre_S * tObj;
    Golden_Vec = Vec_PtrStart(0);
    // int bound = Th_CountLevelWithDummy(vThres);
    // for (i = 0; i < bound; ++i) {
    // for (i = 0; i < 2; ++i) {
    //     Vec_Ptr_t* temp = Vec_PtrStart(0);
    //     Vec_PtrPush(Golden_Vec, temp);
    // }
    // int bound = Th_CountLevelWithDummy(vThres);
    /*printf("bound = %d\n", bound);*/

    Vec_PtrForEachEntry(Thre_S *, vThres, tObj, i)
    {
        if (tObj->Type == Th_CONST1 || tObj->Type == Th_Pi) continue;
        if (!Vec_IntSize(tObj->dtypes)) continue;
        int type,k;
        int temp_type = 0;
        Vec_Int_t * temp_set = Vec_IntStart(0);
        Vec_IntForEachEntry(tObj->dtypes , type , k)
        {
            if(type >= 2)
            {
                Vec_IntClear(temp_set);
                continue;
            }
            if(k == Vec_IntSize(tObj->dtypes)-1 && k > 1)
            {
                // if(temp_type == 2 || type == 2) break;
                if(Vec_IntEntry(tObj->FaninCs,k) == 1)
                    Vec_IntPush(temp_set,((-1)*Vec_IntEntry(tObj->Fanins,k)));
                else
                    Vec_IntPush(temp_set,Vec_IntEntry(tObj->Fanins,k));
                // if(Vec_IntSize(temp_set) > 2) {
                //     printf("BUS\n");
                //     if(temp_type == 0) break;
                // }

                // else printf("hi %d\n",i);
                // assert(Vec_IntSize(temp_set) >= 2 && Vec_IntSize(temp_set) <= Vec_IntSize(tObj->dtypes));

                Th_Set * t = ABC_ALLOC(Th_Set, 1);
                t->set = Vec_IntDup(temp_set);
                Vec_IntSort(t->set,0);
                t->Dtype = temp_type;
                t->gate = tObj->Id;
                // push to space  
                // printf("level = %d\n", tObj->level);
                // if (tObj->level <= bound/2)
                //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 0),t);
                // else 
                //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 1),t);
                // else if (tObj->level <= bound*3/4)  
                //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 2),t); 
                // else
                //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 3),t);
                Vec_PtrPush(Golden_Vec,t);
                // Vec_PtrPush(Vec_PtrEntry(Golden_Vec, tObj->level),t);
                break;  
            }
            if(type != temp_type)
            {
                if(Vec_IntSize(temp_set) > 1 )
                {
                    Th_Set * t = ABC_ALLOC(Th_Set, 1);
                    t->set = Vec_IntDup(temp_set);
                    //t->set = Vec_IntDup(temp_set);
                    Vec_IntSort(t->set,0);
                    t->Dtype = temp_type;
                    t->gate = tObj->Id; 
                    // push to space
                    // printf("level-1 = %d\n", tObj->level);
                    // if (tObj->level <= bound/2)
                    //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 0),t);
                    // else 
                    //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 1),t);
                    // else if (tObj->level <= bound*3/4)  
                    //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 2),t); 
                    // else
                    //     Vec_PtrPush(Vec_PtrEntry(Golden_Vec, 3),t);
                    Vec_PtrPush(Golden_Vec,t);
                    // Vec_PtrPush(Vec_PtrEntry(Golden_Vec, tObj->level),t);
                }
                Vec_IntClear(temp_set);
                temp_type = type;
            }
            if(Vec_IntEntry(tObj->FaninCs,k) == 1)
                Vec_IntPush(temp_set,((-1)*Vec_IntEntry(tObj->Fanins,k)));
            else
                Vec_IntPush(temp_set,Vec_IntEntry(tObj->Fanins,k));
        }
        Vec_IntFree(temp_set);
    }
    // printf("Build Done!\n");
}

int compare(Th_Set** i1, Th_Set** i2) {
  Th_Set* p1 = *i1;
  Th_Set* p2 = *i2;
  int value1 = Vec_IntSize(p1->set);
  int value2 = Vec_IntSize(p2->set);
  if (value1 > value2) return 1;
  else if (value1 == value2) return 0;
  else return -1;
}

