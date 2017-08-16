﻿#include <windows.h>
#include "hash.h"
tableData* __cdecl _rehash(tableData** table, pfnLib lib){
	unsigned int newLength = (*table)->tableSizes[(*table)->nextLenInd]; 	// get the new length
	if (newLength == 0)														// fail
		return 0;
	tableData* newTable = lib->pnewHashTable(newLength,lib->pmalloc,(*table)->tableSizes,(*table)->maxLoad,(*table)->nextLenInd+1);	// set up the new table struct
	if (newTable==0)	// fail
		return 0;
	newTable->numkeys=(*table)->numkeys;	// number of keys are not changed by rehash
	unsigned int i;
	unsigned int k,p;	// k, key strlen, m loop ind
	unsigned int hash;	// ∑_i key[i]*pow(31,i), i ∈ [0,k-1] 
	
	node* oldNode;	// node place holders
	node* nextNode;
	for (i=0; i<(*table)->length; ++i){	// visit all nodes
		oldNode = (*table)->nodes[i];
		
		while (oldNode!=0) {
			node* newNode = (node*) lib->pmalloc(sizeof(node));	// make a new node 
			newNode->next=0;									// copy old values to new node
			newNode->key=oldNode->key;
			newNode->val=oldNode->val;
			oldNode=oldNode->next;
			for (k=0,p=1,hash=0; newNode->key[k]!=0;++k){		// calculate key hash for the new table
				hash+=newNode->key[k]*p;
				p*=31;
			}
			hash =  hash % newLength;							// map to valid index range
			
			
			nextNode = newTable->nodes[hash];					
			if (nextNode==0){					// place the new node first in the bucket if no nodes there...
				newTable->nodes[hash]=newNode;
				continue;
			}
			while (nextNode->next!=0)			// ... else go to the end of bucket and place it there.
				nextNode=nextNode->next;
			nextNode->next=newNode;
			
		}
	}
	// free old nodes.
	for (i=0; i<(*table)->length; ++i){
		oldNode = (*table)->nodes[i]; 
		while (oldNode!=0){
			nextNode=oldNode->next;
			lib->pfree(oldNode);
			oldNode=nextNode;
		}
	}
	// free the old table.
	lib->pfree((*table)->nodes);
	lib->pfree((*table));
	return newTable; // done
}