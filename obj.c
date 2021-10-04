#include "obj.h"

enum type {
	COL = 1,
	NOCOL = 2
};

struct quad {
	int32_t x[4];
	int32_t y[4];
	int32_t z[4];

	enum type type;
	int32_t texture;
};

struct vertice {
	int32_t x;
	int32_t y;
	int32_t z;
};

int32_t next_char_pointer(char *content, int32_t current_offset, int32_t lim, char delim) {
	// Returns the index of the next newline char
	int32_t i = 0;
	
	while ((*(content + current_offset + i) != delim) && i + current_offset < lim) {
		i += 1;
	}	

	return current_offset + i;
};

int32_t main(int32_t argc, char *argv[]) {
	if (argc < 3) {
		printf("No infile or outfile supplied");
		return 0;
	}
	in_file = argv[1];
	out_file = argv[2];	
	
	printf("Read: %s. Write: %s\n", in_file, out_file);

	// Allocate buffer for file size
	FILE *fp;
	fp = fopen(in_file, "r");

	if (fp == NULL) {
		perror("Failed to open infile");
		return 0;
	}

	fseek(fp, 0L, SEEK_END);
	int32_t file_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	printf("%s %d bytes long\n", in_file, file_size);
	char fp_content[file_size];

	fread(&fp_content, file_size, 1, fp);
	fclose(fp);

	// Count vertexes
	int32_t vertex_count = 0;
	int32_t face_count = 0;
	int32_t offset = 0;

	while (offset < file_size) {
		int32_t line_size = next_char_pointer((char *) &fp_content, offset, file_size, (char) *"\n");
		line_size -= offset;

		char *line = (char*) malloc(line_size + 2);
		memset(line, '\x00', line_size);
		memcpy(line, (char *) &fp_content[offset], line_size);
		
		if (line[0] == *"v" && line[1] == *" ") {
			vertex_count += 1;
		} else if (line[0] == *"f") {
			// Face
			face_count += 1;
		}

		free(line);
		offset += line_size + 1;
	}
	printf("Vertex count: %d\n", vertex_count);
	printf("Face count: %d\n", face_count);

	// Allocate the array for the vertices
	struct vertice object_vertices[vertex_count];
	offset = 0;
	int32_t vertex_i = 0;

	// Allocate the array for the faces
	face faces[face_count];
	int32_t face_i = 0;

	while (offset < file_size) {
		int32_t line_size = next_char_pointer((char *) &fp_content, offset, file_size, (char) *"\n");
		line_size -= offset;

		char *line = (char*) malloc(line_size + 2);
		memset(line, '\x00', line_size);
		memcpy(line, (char *) &fp_content[offset], line_size);
		
		if (line[0] == *"v" && line[1] == *" ") {
			// Interperate vertex
			int32_t col_offset = 2;
			int32_t v_size;

			v_size = next_char_pointer((char*) &fp_content, offset + col_offset, file_size, (char) *" "); 			
			v_size -= offset;
			v_size -= col_offset;

			char *x_str = (char*) malloc(v_size + 2);
			memset(x_str, '\x00', v_size);
			memcpy(x_str, (char*) &fp_content[offset + col_offset], v_size);
			object_vertices[vertex_i].x = atoi(x_str) * scale;
			free(x_str);

			col_offset += v_size + 1;
			v_size = next_char_pointer((char*) &fp_content, offset + col_offset, file_size, (char) *" ");
			v_size -= offset;
			v_size -= col_offset;

			char *y_str = (char*) malloc(v_size + 2);
			memset(y_str, '\x00', v_size);
			memcpy(y_str, (char*) &fp_content[offset + col_offset], v_size);
			object_vertices[vertex_i].y = atoi(y_str) * scale;
			free(y_str);

			col_offset += v_size + 1;
			v_size = next_char_pointer((char*) &fp_content, offset + col_offset, file_size, (char) *"\n");
			v_size -= offset;
			v_size -= col_offset;

			char *z_str = (char*) malloc(v_size + 2);
			memset(z_str, '\x00', v_size);
			memcpy(z_str, (char*) &fp_content[offset + col_offset], v_size);
			object_vertices[vertex_i].z = atoi(z_str) * scale;
			free(z_str);

			vertex_i += 1;
		}

		if (line[0] == *"f" && line[1] == *" ") { // Face
			int32_t col_offset = 2;
			int32_t v_size;
			int32_t end_ptr;

			end_ptr = next_char_pointer((char*) &fp_content, offset + col_offset, file_size, (char) *"\n");
			int32_t space_count = 0;

			for (int32_t i = col_offset + offset; i < end_ptr; i += 1) {
				if (fp_content[i] == *" ") {
					space_count += 1;
				}
			}

			// Count faces
			if (space_count + 1 > 4) {
				printf("This object containes more than 4 vertices per face and cannot be converted to the .ssv format\nAborting\n");
				return 255;
			}

			char *str_buffer = (char*) malloc(12);
			memset(str_buffer, '\x00', 12);
			int32_t next_char;
			
			for (int32_t f_vx_count = 0; f_vx_count < space_count; f_vx_count += 1) {
				next_char = next_char_pointer((char*) &fp_content, offset + col_offset, file_size, (char) *" ");

				memcpy(str_buffer, (char*) &fp_content[offset + col_offset], next_char - (offset + col_offset));

				col_offset += next_char - (offset + col_offset) + 1;
				faces[face_i][f_vx_count] = atoi(str_buffer);
				memset(str_buffer, '\x00', 12);
			}
			next_char = next_char_pointer((char*) &fp_content, offset + col_offset, file_size, (char) *"\n");

			memcpy(str_buffer, (char*) &fp_content[offset + col_offset], next_char - (offset + col_offset));

			col_offset += next_char - (offset + col_offset) + 1;
			faces[face_i][space_count] = atoi(str_buffer);
			face_i += 1;

			free(str_buffer);
		}

		// Free the memory to avoid chrome levels of ram usage
		free(line);
		offset += line_size + 1;
	}

	// We can now combine the faces and vertices into a quad struct array
	struct quad quads[face_count];
	for (int32_t i = 0; i < face_count; i += 1) {
		for (int32_t ii = 0; ii < 4; ii += 1) {
			quads[i].x[ii] = object_vertices[(faces[i][ii]) - 1].x;
			quads[i].y[ii] = object_vertices[(faces[i][ii]) - 1].y;
			quads[i].z[ii] = object_vertices[(faces[i][ii]) - 1].z;
			
			// Flip the bits
			quads[i].x[ii] = ((quads[i].x[ii]>>24)&0xff) | // move byte 3 to byte 0
                    ((quads[i].x[ii]<<8)&0xff0000) | // move byte 1 to byte 2
                    ((quads[i].x[ii]>>8)&0xff00) | // move byte 2 to byte 1
                    ((quads[i].x[ii]<<24)&0xff000000); // byte 0 to byte 3
				
			quads[i].y[ii] = ((quads[i].y[ii]>>24)&0xff) | // move byte 3 to byte 0
                    ((quads[i].y[ii]<<8)&0xff0000) | // move byte 1 to byte 2
                    ((quads[i].y[ii]>>8)&0xff00) | // move byte 2 to byte 1
                    ((quads[i].y[ii]<<24)&0xff000000); // byte 0 to byte 3
			
			quads[i].z[ii] = ((quads[i].z[ii]>>24)&0xff) | // move byte 3 to byte 0
                    ((quads[i].z[ii]<<8)&0xff0000) | // move byte 1 to byte 2
                    ((quads[i].z[ii]>>8)&0xff00) | // move byte 2 to byte 1
                    ((quads[i].z[ii]<<24)&0xff000000); // byte 0 to byte 3

			printf("q: %d, v: %d, fi: %d, xyz: %d, %d, %d\n", i, ii, (faces[i][ii]) - 1, object_vertices[(faces[i][ii]) - 1].x, object_vertices[(faces[i][ii]) - 1].y, object_vertices[(faces[i][ii]) - 1].z);
		}
	}

	int32_t uface_found = face_count;
	uface_found = ((uface_found>>24)&0xff) | // move byte 3 to byte 0
                    ((uface_found<<8)&0xff0000) | // move byte 1 to byte 2
                    ((uface_found>>8)&0xff00) | // move byte 2 to byte 1
                    ((uface_found<<24)&0xff000000); // byte 0 to byte 3

	fp = fopen(out_file, "w");

	fwrite(&uface_found, 1, sizeof(int32_t), fp);
	fwrite(&quads, 1, sizeof(struct quad)*uface_found, fp);

	fclose(fp);

	printf("File saved: %s\n", out_file);
	printf("Byts written: %d\n", sizeof(int32_t) + sizeof(struct quad)*face_count);

	return 0;
}
