#include <id3.h>

void set_TagField(ID3Tag *myTag,char *data,ID3_FrameID id){
	ID3Frame *myFrame;
	ID3Frame *pFrame;
	
	myFrame=ID3Frame_NewID(id);

	pFrame=ID3Tag_FindFrameWithID(myTag,id);

	if (pFrame != NULL) {
		ID3Tag_RemoveFrame(myTag,pFrame);
	}

	ID3Field_SetASCII(ID3Frame_GetField(myFrame,ID3FN_TEXT),data);
	ID3Tag_AttachFrame(myTag,myFrame);

	return;
}
