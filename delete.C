#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
// part 6

if (relation.empty()) return BADCATPARM; //check if relation is empty

RID rid;
AttrDesc desc;
HeapFileScan *scanner;
Status status;

// **TODO**:try scanner init here
scanner = new HeapFileScan(relation, status);
if (status != OK) return status;

if (attrName.empty()){
	//scanner = new HeapFileScan(relation, status);
	//if (status != OK) return status;

	scanner->startScan(0, 0, STRING, NULL, EQ); //starting scanner

	while ((status = scanner->scanNext(rid)) != FILEEOF){
		status = scanner->deleteRecord();
		if (status != OK) return status;
	}
} else {
	//scanner = new HeapFileScan(relation, stauts);
	//if (status != OK) return status;

	status = attrCat->getInfo(relation, attrName, desc);
	if (status != OK) return status;

	//check data type for scan
	int iNum;
	float fNum;
	if (type == FLOAT){
		fNum = atof(attrValue);
		status = scanner->startScan(desc.attrOffset, desc.attrLen, type, (char *)&fNum, op);
	} else if (type == INTEGER){
		iNum = atoi(attrValue);
		status = scanner->startScan(desc.attrOffset, desc.attrLen, type, (char *)&iNum, op);
	} else {
		status = scanner->startScan(desc.attrOffset, desc.attrLen, type, attrValue, op);
	}
	if (status != OK) return status;

	while ((status = scanner->scanNext(rid)) != FILEEOF){
		status = scanner->deleteRecord();
		if (status != OK) return status;
	}
}

//**TODO**: IDT WE NEED THIS
if (status != FILEEOF) return status;

//check status shit before submit
status = scanner->endScan();
delete scanner;

return status;
}


