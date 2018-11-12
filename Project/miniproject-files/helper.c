float getNumFromString(char* str, char* sub){
	float num = 0;

	if ((str,sub) == NULL) {

	} else {
		num = atof(str + strlen(sub));
	}

	return num;
}