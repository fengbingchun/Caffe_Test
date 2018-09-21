#include "hdf5_test.hpp"
#include <iostream>
#include <hdf5.h>

#ifdef _MSC_VER
#define H5FILE_NAME		"E:/GitCode/Caffe_Test/test_data/third_party_library/SDS.h5"
#else
#define H5FILE_NAME		"test_data/third_party_library/SDS.h5"
#endif
#define DATASETNAME		"IntArray"
#define NX			5 /* dataset dimensions */
#define NY			6
#define RANK			2

static int test_HDF5_write_HDF5_Data()
{
	hid_t       file, dataset;         /* file and dataset handles */
	hid_t       datatype, dataspace;   /* handles */
	hsize_t     dimsf[2];              /* dataset dimensions */
	herr_t      status;
	int         data[NX][NY];          /* data to write */
	int         i, j;

	//Data  and output buffer initialization.
	for (j = 0; j < NX; j++)
	for (i = 0; i < NY; i++)
		data[j][i] = i + j + 100;//changed
	/*
	* 0 1 2 3 4 5
	* 1 2 3 4 5 6
	* 2 3 4 5 6 7
	* 3 4 5 6 7 8
	* 4 5 6 7 8 9
	*/

	/*
	* Create a new file using H5F_ACC_TRUNC access,
	* default file creation properties, and default file
	* access properties.
	*/
	file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	/*
	* Describe the size of the array and create the data space for fixed
	* size dataset.
	*/
	dimsf[0] = NX;
	dimsf[1] = NY;
	dataspace = H5Screate_simple(RANK, dimsf, NULL);

	/*
	* Define datatype for the data in the file.
	* We will store little endian INT numbers.
	*/
	datatype = H5Tcopy(H5T_NATIVE_INT);
	status = H5Tset_order(datatype, H5T_ORDER_LE);

	/*
	* Create a new dataset within the file using defined dataspace and
	* datatype and default dataset creation properties.
	*/
	dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	//Write the data to the dataset using default transfer properties.
	status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	//Close/release resources.
	H5Sclose(dataspace);
	H5Tclose(datatype);
	H5Dclose(dataset);
	H5Fclose(file);

	return 0;
}

#define NX_SUB			3 /* hyperslab dimensions */
#define NY_SUB			4
#define NX			7 /* output buffer dimensions */
#define NY			7
#define NZ			3
#define RANK			2
#define RANK_OUT		3

static int test_HDF5_read_HDF5_data()
{
	hid_t       file, dataset;         /* handles */
	hid_t       datatype, dataspace;
	hid_t       memspace;
	H5T_class_t t_class;                 /* data type class */
	H5T_order_t order;                 /* data order */
	size_t      size;                  /*
					   * size of the data element
					   * stored in file
					   */
	hsize_t     dimsm[3];              /* memory space dimensions */
	hsize_t     dims_out[2];           /* dataset dimensions */
	herr_t      status;

	int         data_out[NX][NY][NZ]; /* output buffer */

	hsize_t      count[2];              /* size of the hyperslab in the file */
	hsize_t      offset[2];             /* hyperslab offset in the file */
	hsize_t      count_out[3];          /* size of the hyperslab in memory */
	hsize_t      offset_out[3];         /* hyperslab offset in memory */
	int          i, j, k, status_n, rank;

	for (j = 0; j < NX; j++) {
		for (i = 0; i < NY; i++) {
			for (k = 0; k < NZ; k++)
				data_out[j][i][k] = 0 - 1000;//changed
		}
	}

	/*
	* Open the file and the dataset.
	*/
	file = H5Fopen(H5FILE_NAME, H5F_ACC_RDONLY, H5P_DEFAULT);
	dataset = H5Dopen2(file, DATASETNAME, H5P_DEFAULT);

	/*
	* Get datatype and dataspace handles and then query
	* dataset class, order, size, rank and dimensions.
	*/
	datatype = H5Dget_type(dataset);     /* datatype handle */
	t_class = H5Tget_class(datatype);
	if (t_class == H5T_INTEGER) printf("Data set has INTEGER type \n");
	order = H5Tget_order(datatype);
	if (order == H5T_ORDER_LE) printf("Little endian order \n");

	size = H5Tget_size(datatype);
	printf("Data size is %d \n", (int)size);

	dataspace = H5Dget_space(dataset);    /* dataspace handle */
	rank = H5Sget_simple_extent_ndims(dataspace);
	status_n = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
	printf("rank %d, dimensions %lu x %lu \n", rank,
		(unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]));

	/*
	* Define hyperslab in the dataset.
	*/
	offset[0] = 1;
	offset[1] = 2;
	count[0] = NX_SUB;
	count[1] = NY_SUB;
	status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL,
		count, NULL);

	/*
	* Define the memory dataspace.
	*/
	dimsm[0] = NX;
	dimsm[1] = NY;
	dimsm[2] = NZ;
	memspace = H5Screate_simple(RANK_OUT, dimsm, NULL);

	/*
	* Define memory hyperslab.
	*/
	offset_out[0] = 3;
	offset_out[1] = 0;
	offset_out[2] = 0;
	count_out[0] = NX_SUB;
	count_out[1] = NY_SUB;
	count_out[2] = 1;
	status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL,
		count_out, NULL);

	/*
	* Read data from hyperslab in the file into the hyperslab in
	* memory and display.
	*/
	status = H5Dread(dataset, H5T_NATIVE_INT, memspace, dataspace,
		H5P_DEFAULT, data_out);
	for (j = 0; j < NX; j++) {
		for (i = 0; i < NY; i++) printf("%d ", data_out[j][i][0]);
		printf("\n");
	}
	/*
	* 0 0 0 0 0 0 0
	* 0 0 0 0 0 0 0
	* 0 0 0 0 0 0 0
	* 3 4 5 6 0 0 0
	* 4 5 6 7 0 0 0
	* 5 6 7 8 0 0 0
	* 0 0 0 0 0 0 0
	*/

	/*
	* Close/release resources.
	*/
	H5Tclose(datatype);
	H5Dclose(dataset);
	H5Sclose(dataspace);
	H5Sclose(memspace);
	H5Fclose(file);

	return 0;
}

int test_hdf5_1()
{
	/* reference:
		http://wenku.baidu.com/link?url=HDnqbrqqJ27GSvVGTcCbfM-bn5K2QCYxSlqTEtY_jwFvBVi3B97DK6s9qBUwXDjVgMHFQq-MLGSKcMKeGJkq87GF_8vchhsleRWISq9PwO3
		http://baike.baidu.com/link?url=TqYZDUzu_XFMYa9XswMS1OVSyboWzu3RtK6L-DiOFZT6zugtXjBUIFa4QHerxZcSbPNuTO84BomEGgxpchWojK
		http://www.docin.com/p-608918978.html
	*/
	// 1-->./examples/h5_write.c:This example writes data to the HDF5 file
	test_HDF5_write_HDF5_Data();
	// 2-->./examples/h5_read.c:This example reads hyperslab from the SDS.h5 file
	test_HDF5_read_HDF5_data();

	return 0;
}
