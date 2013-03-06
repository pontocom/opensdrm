#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "zip_header.h"
#include "zip_inflate.h"
#include "zip_wrapper.h"
#include "zip_testcrc.h"

ZipFile* zip_file_new(void)
{
	ZipFile *zip;

	zip = new ZipFile;
        if (!zip)
            return NULL;

	zip->filename = 0;
	zip->cdre = 0;
	zip->records = new vector<ZipCDR *>;
	zip->opened = 0;

	return zip;
}

int zip_file_open(const char *filename,ZipFile *zip)
{
	FILE *file;
	int status;

	ZipCDREnd *e;

	file = fopen(filename,"rb");
        if (!file)
            return 0;

	status=zip_find_end_of_central_dir_record(file);
	if (status == 0) {
            fclose(file);
	    return 0;
        }

	e = new ZipCDREnd;
        if (!e) {
            fclose(file);
            return 0;
        }
	
	status = zip_read_end_of_central_dir_record(file,e);
        if (!status) {
            fclose(file);
            return 0;
        }

	zip->cdre = e;

	status = zip_goto_central_dir_record_start(file,e);
        if (!status) {
            fclose(file);
            return 0;
        }

	status=zip_file_add_records_with_file(file,zip);

	if (status != 0)
		zip->opened=1;
	else {
		free(zip->cdre);
		zip->cdre=0;
		zip->opened=0;
	}

	fclose(file);

	zip->filename = new char[strlen(filename) + 1];
        strcpy(zip->filename, filename);
        if (!zip->filename)
            return 0;

	return status;
}

/* zip_file_add_records()
   Function adds central dir records to single linked list */

int zip_file_add_records_with_file(FILE *file,ZipFile *zip) /* 0 = failure */
{
	unsigned int a;
	int status;
	ZipCDR *r;

        if (!file || !zip)
            return 0;

	for (a = 0; a < zip->cdre->totalentriesonthisdisk; a++) {
                r = new ZipCDR;
                if (!r)
                    return 0;

		status = zip_read_central_dir_record(file,r);

		if (status != 0) 
                        zip->records->push_back(r);
		else 
			cerr << "Error when reading cdrs\n";
	}
	return 1;
}

int zip_file_add_records(ZipFile *zip) /* 0 = failure */
{
	FILE *file;
	unsigned int a;
	int status;
	ZipCDR *r;

        if (!zip || !zip->filename)
            return 0;

	file = fopen(zip->filename,"rb");
        if (!file)
            return 0;

	for (a = 0; a < zip->cdre->totalentriesonthisdisk; a++) {
                r = new ZipCDR;
                if (!r) 
                    return 0;
		
		status = zip_read_central_dir_record(file, r);

		if (status!=0) 
                        zip->records->push_back(r);
		else 
			cerr << "Error when reading cdrs\n";
	}
	fclose(file);
	return 1;
}

void zip_file_free_current(ZipCDR *data)
{
	if (data==NULL)
		return;

	if (data->filename)
		free(data->filename);

	data->filename=NULL;
}

int zip_file_free(ZipFile *zip)
{
        if (!zip)
            return 0;

	if (zip->filename)
		delete zip->filename;
	zip->filename=NULL;

	zip->opened=0;

	if (zip->cdre != NULL)
		delete zip->cdre;

        vector<ZipCDR *>::iterator i = zip->records->begin();
        for (; i != zip->records->end(); i++) {
            zip_file_free_current(*i);
            delete *i;
        }

        delete zip->records;
	zip->records=NULL;
	
	delete zip;
	zip=NULL;

	return 1;
}

int zip_file_close(ZipFile *zip)
{
        if (!zip)
            return 0;

	if (zip->filename)
		delete zip->filename;
	zip->filename=NULL;

	zip->opened=0;

	if (zip->cdre != NULL) {
		delete zip->cdre;
		zip->cdre=NULL;
	}

        vector<ZipCDR *>::iterator i = zip->records->begin();
        for (; i != zip->records->end(); i++) {
            zip_file_free_current(*i);
            delete *i;
        }

	delete zip->records;
        zip->records = new vector<ZipCDR *>;

	return 1;
}

/* zip_file_uncompress_all()
   extra_flags:
    0x01 - skip errors
    0x02 - lowercase names */

int zip_file_uncompress_all(const char *dir,ZipFile *zip,int extra_flags)
{
	ZipFileVars vars;
	FILE *file;

        if (!dir || !zip)
            return 0;

	vars.skip_errors = extra_flags;
	vars.dir = dir;
	vars.error_count = 0;

	file = fopen(zip->filename,"rb");
        if (!file)
            return 0;

	vars.file = file;

        vector<ZipCDR *>::iterator i = zip->records->begin();
        for (; i != zip->records->end(); i++) {
            zip_file_uncompress_current(*i, &vars);
        }
        
	if (vars.error_count>0)
		return -vars.error_count;

	fclose(file);

	return 1;
}

int zip_file_uncompress_nth(unsigned int n,const char *dir,ZipFile *zip,
			    int extra_flags)
{
	ZipFileVars vars;
	FILE *file;
	ZipCDR *entry;

        if (!dir || !zip)
            return 0;

	vars.skip_errors = extra_flags;
	vars.dir = dir;
	vars.error_count = 0;

	file = fopen(zip->filename,"rb");
        if (!file)
            return 0;

        entry = (*zip->records)[n];
        if (!entry) {
            fclose(file);
            return 0;
        }

	vars.file = file;

	zip_file_uncompress_current(entry, &vars);

	if (vars.error_count>0)
		return -vars.error_count;

	fclose(file);

	return 1;
}

void zip_file_uncompress_current(ZipCDR *r, ZipFileVars *vars)
{
	int status=0;

	if (!r || !vars)
            return;
	
	status=uncompress_file_with_dir(r, vars->file, vars->dir,
					vars->skip_errors);

	if (status == 0)
		vars->error_count++;
}

vector<char *> *zip_file_test(ZipFile *zip,int *status)
{
	ZipFileVars vars;
	FILE *file;

        if (!zip)
            return NULL;

	vars.skip_errors = 1;
	vars.error_count = 0;
        vars.errors = new vector<char *>;
        if (!vars.errors)
            return NULL;

	file = fopen(zip->filename,"rb");
        if (!file)
            return NULL;

	vars.file = file;

        vector<ZipCDR *>::iterator i = zip->records->begin();
        for (; i != zip->records->end(); i++) {
            zip_file_test_current(*i, &vars);
        }
 
	fclose(file);

	if (vars.error_count>0) {
		*status=0;
	} else 
		*status=1;

	return vars.errors;
}

void zip_file_test_current(ZipCDR *r, ZipFileVars *vars)
{
	int status=0;

	if (!r || !vars)
		return;

	status = zip_test_crc(r,vars->file);
	if (status==0) {
		vars->error_count++;
		vars->errors->push_back(r->filename);
	}
}
