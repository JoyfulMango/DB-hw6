#include "catalog.h"
#include "query.h"
#include "stdio.h"
#include "stdlib.h"


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
   // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;

	AttrDesc *desc = NULL;	
	AttrDesc attrDescArray[projCnt]; 
	Status status;
	int rlen = 0;
	int i;

	for (int i = 0; i <projCnt; i++){
		status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, attrDescArray[i]);
		if (status != OK) return status;
	}

	if (attr != NULL){
		desc = new AttrDesc;
		status = attrCat->getInfo(attr->relName, attr->attrName, *desc);
		if (status != OK) return status;
	}

	for (int i = 0; i < projCnt; i++){
		rlen += attrDescArray[i].attrLen;
	}

	return ScanSelect(result, projCnt, attrDescArray, desc, op, attrValue, rlen);
}


const Status ScanSelect(const string & result, 
#include "stdio.h"
#include "stdlib.h"
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;

	
	Record rout;
	RID rid;
	Record rec;
	Status status;

	InsertFileScan resultRel(result, status);
	if (status != OK) return status;

	rout.length = reclen;
	rout.data = (char *)malloc(reclen);

	//starting scan
	int iVal;
	float fVal;
	
	HeapFileScan scan(projNames[0].relName, status);
	if (status != OK) return status;

	if (attrDesc == NULL){
		status = scan.startScan(0, 0, STRING, NULL, EQ);
	} else if(attrDesc->attrType == STRING){
		status = scan.startScan(attrDesc->attrOffset, attrDesc->attrLen, STRING, filter, op);
	} else if(attrDesc->attrType == INTEGER){
		iVal = atoi(filter);
		status = scan.startScan(attrDesc->attrOffset, attrDesc->attrLen, INTEGER, (char *)&iVal, op);
	}	else if(attrDesc->attrType == FLOAT){
		fVal = atof(filter);
		status = scan.startScan(attrDesc->attrOffset, attrDesc->attrLen, FLOAT, (char *)&fVal, op);
	}
	if (status != OK) return status;

	while (scan.scanNext(rid) == OK)
	{
		status = scan.getRecord(rec);
		if (status != OK) return status;

		int outOffset = 0;
		for (int i = 0; i < projCnt; i++)
		{
			memcpy((char *)rout.data + outOffset, (char *)rec.data + projNames[i].attrOffset, projNames[i].attrLen);
			outOffset += projNames[i].attrLen;
		}

		RID outRID;
		status = resultRel.insertRecord(rout, outRID);
	}

	if (status != FILEEOF) return status;

	status = scan.endScan();
	if (status != OK) return status;
	return OK;
}
