






decltype(auto) get_file_perms(const std::filesystem::path &p){
	return std::filesystem::status(p).permissions();
}

decltype(auto) is_file_perms_equal_to_all(const std::filesystem::path &p){
	return get_file_perms(p) == std::filesystem::all;
}

void set_file_perms_to_readonly(const std::filesystem::path &p){
	std::filesystem::permissions(p, std::filesystem::owner_read | std::filesystem::group_read | std::filesystem::others_read);//? should work on windows?
}

void set_file_perms_to_all(const std::filesystem::path &p){
	std::filesystem::permissions(p, std::filesystem::all);
}


decltype(auto) prepare_for_ipc_file_std_streams(void){
	decltype(auto) ofile = create_tmp_file();
	decltype(auto) ifile = create_tmp_file();
	set_file_perms_to_all(ofile);
	set_file_perms_to_all(ifile);
	//? filesystem::resize_file to resize both files to constant size here?
	return make_tuple(ofile, ifile);
}

//template<class T>
//void ipc_write_to_file(T data){
//	//undone
//}
//
//void ipc_finish_writing_to_file(const std::filesystem::path &p){
//	//
//	set_file_perms_to_readonly(p);
//	//todo write to child process stdin / write to stdout using uv_spawn and uv_read_start?
//}
