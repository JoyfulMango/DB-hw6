#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
	// part 6

	AttrDesc *desc;
    Record rec;
    RID rid;
	Status status;
    int rAttrCnt;
    int rLen;
    int iVal;
    int fVal;
	
	status = attrCat->getRelInfo(relation, rAttrCnt, desc);
	if(status != OK) return status;

	if(rAttrCnt != attrCnt) return OK; //wrong attribute count

	for (int i = 0; i < attrCnt; i++){
		rLen += desc[i].attrLen;
	}

	InsertFileScan resultRel(relation, status);
	if(status != OK) return status;

	rec.length = rLen;
	rec.data = (char *)malloc(rLen);

	int i = 0;
	while (i < rAttrCnt){
		for (int j; j< attrCnt; j++ ){
			if(strcmp(attrList[j].attrName, desc[i].attrName) == 0){
				//Datatype type = attrList[j].attrType;
				if(attrList[j].attrType == FLOAT){
					fVal = atof((char *)attrList[j].attrValue);
					memcpy((char *)rec.data + desc[i].attrOffset, (char *)&fVal, desc[i].attrLen);
				} else if(attrList[j].attrType == INTEGER){
					iVal = atoi((char *)attrList[j].attrValue);
					memcpy((char *)rec.data + desc[i].attrOffset, (char *)&iVal, desc[i].attrLen);
				} else {
					memcpy((char *)rec.data + desc[i].attrOffset, attrList[j].attrValue, desc[i].attrLen);
				}
			}
		}
		i++;
	}
	resultRel.insertRecord(rec, rid);

	return OK;

}

