#!/usr/bin/env python
# MachSuite benchmark definitions.

from design_sweep_types import *

aes_aes = Benchmark("aes-aes", "aes", "common/harness.c")
aes_aes.set_kernels(["aes256_encrypt_ecb"])
aes_aes.set_main_id(0x00000010)
aes_aes.add_array("ctx", 96, 1, PARTITION_CYCLIC)
aes_aes.add_array("k", 32, 1, PARTITION_CYCLIC)
aes_aes.add_array("buf", 16, 1, PARTITION_CYCLIC)
aes_aes.add_array("rcon", 1, 1, PARTITION_COMPLETE)
aes_aes.add_array("sbox", 256, 1, PARTITION_CYCLIC)
aes_aes.add_loop("aes_addRoundKey_cpy", "cpkey", 16)
aes_aes.add_loop("aes_subBytes", "sub", 16)
aes_aes.add_loop("aes_addRoundKey", "addkey", 16)
aes_aes.add_loop("aes256_encrypt_ecb", "ecb1", 32)
aes_aes.add_loop("aes256_encrypt_ecb", "ecb2", 8)
aes_aes.add_loop("aes256_encrypt_ecb", "ecb3", 13)
aes_aes.set_exec_cmd("%(source_dir)s/aes/aes/aes-aes-gem5-accel")
aes_aes.set_run_args("%(source_dir)s/aes/aes/input.data %(source_dir)s/aes/aes/check.data")

bfs_bulk = Benchmark("bfs-bulk", "bulk", "common/harness.c")
bfs_bulk.set_kernels(["bfs"])
bfs_bulk.set_main_id(0x00000030)
bfs_bulk.add_array("nodes", 512, 8, PARTITION_CYCLIC)
bfs_bulk.add_array("edges", 4096, 8, PARTITION_CYCLIC)
bfs_bulk.add_array("level", 256, 1, PARTITION_CYCLIC)
bfs_bulk.add_array("level_counts", 10, 8, PARTITION_CYCLIC)
bfs_bulk.add_loop("bfs", "loop_horizons", UNROLL_ONE)
bfs_bulk.add_loop("bfs", "loop_nodes", 256)
bfs_bulk.add_loop("bfs", "loop_neighbors", UNROLL_FLATTEN)
bfs_bulk.set_exec_cmd("%(source_dir)s/bfs/bulk/bfs-bulk-gem5-accel")
bfs_bulk.set_run_args("%(source_dir)s/bfs/bulk/input.data %(source_dir)s/bfs/bulk/check.data")

bfs_queue = Benchmark("bfs-queue", "queue", "common/harness.c")
bfs_queue.set_kernels(["bfs"])
bfs_queue.set_main_id(0x00000040)
bfs_queue.add_array("queue", 256, 8, PARTITION_CYCLIC)
bfs_queue.add_array("nodes", 512, 8, PARTITION_CYCLIC)
bfs_queue.add_array("edges", 4096, 8, PARTITION_CYCLIC)
bfs_queue.add_array("level", 256, 1, PARTITION_CYCLIC)
bfs_queue.add_array("level_counts", 10, 8, PARTITION_CYCLIC)
bfs_queue.add_loop("bfs", "loop_queue", 10)
bfs_queue.add_loop("bfs", "loop_neighbors", UNROLL_ONE)
bfs_queue.set_exec_cmd("%(source_dir)s/bfs/queue/bfs-queue-gem5-accel")
bfs_queue.set_run_args("%(source_dir)s/bfs/queue/input.data %(source_dir)s/bfs/queue/check.data")

fft_strided = Benchmark("fft-strided", "fft", "common/harness.c")
fft_strided.set_kernels(["fft"])
fft_strided.set_main_id(0x00000050)
fft_strided.add_array("real", 1024, 8, PARTITION_CYCLIC)
fft_strided.add_array("img", 1024, 8, PARTITION_CYCLIC)
fft_strided.add_array("real_twid", 1024, 8, PARTITION_CYCLIC)
fft_strided.add_array("img_twid", 1024, 8, PARTITION_CYCLIC)
fft_strided.add_loop("fft", "outer", 512)
fft_strided.add_loop("fft", "inner", 1)
fft_strided.set_exec_cmd("%(source_dir)s/fft/strided/fft-strided-gem5-accel")
fft_strided.set_run_args("%(source_dir)s/fft/strided/input.data %(source_dir)s/fft/strided/check.data")

fft_transpose = Benchmark("fft-transpose", "fft", "common/harness.c")
fft_transpose.set_kernels(["fft1D_512"])
fft_transpose.set_main_id(0x00000060)
fft_transpose.add_array("fft1D_512.reversed", 8, 4, PARTITION_COMPLETE)
fft_transpose.add_array("DATA_x", 512, 8, PARTITION_CYCLIC)
fft_transpose.add_array("DATA_y", 512, 8, PARTITION_CYCLIC)
fft_transpose.add_array("data_x", 8, 8, PARTITION_COMPLETE)
fft_transpose.add_array("data_y", 8, 8, PARTITION_COMPLETE)
fft_transpose.add_array("smem", 576, 8, PARTITION_CYCLIC)
fft_transpose.add_array("work_x", 512, 8, PARTITION_CYCLIC)
fft_transpose.add_array("work_y", 512, 8, PARTITION_CYCLIC)
fft_transpose.add_loop("fft1D_512", "loop1", 64)
fft_transpose.add_loop("fft1D_512", "loop2", 64)
fft_transpose.add_loop("fft1D_512", "loop3", 64)
fft_transpose.add_loop("fft1D_512", "loop4", 64)
fft_transpose.add_loop("fft1D_512", "loop5", 64)
fft_transpose.add_loop("fft1D_512", "loop6", 64)
fft_transpose.add_loop("fft1D_512", "loop7", 64)
fft_transpose.add_loop("fft1D_512", "loop8", 64)
fft_transpose.add_loop("fft1D_512", "loop9", 64)
fft_transpose.add_loop("fft1D_512", "loop10", 64)
fft_transpose.add_loop("fft1D_512", "loop11", 64)
fft_transpose.add_loop("fft1D_512", "twiddles", 8)
fft_transpose.set_exec_cmd("%(source_dir)s/fft/transpose/fft-transpose-gem5-accel")
fft_transpose.set_run_args("%(source_dir)s/fft/transpose/input.data %(source_dir)s/fft/transpose/check.data")

gemm_blocked = Benchmark("gemm-blocked", "bbgemm", "common/harness.c")
gemm_blocked.set_kernels(["bbgemm"])
gemm_blocked.set_main_id(0x00000070)
gemm_blocked.add_array("m1", 4096, 4, PARTITION_CYCLIC)
gemm_blocked.add_array("m2", 4096, 4, PARTITION_CYCLIC)
gemm_blocked.add_array("prod", 4096, 4, PARTITION_CYCLIC)
gemm_blocked.add_loop("bbgemm", "loopjj", UNROLL_ONE)
gemm_blocked.add_loop("bbgemm", "loopkk", UNROLL_ONE)
gemm_blocked.add_loop("bbgemm", "loopi", 64)
gemm_blocked.add_loop("bbgemm", "loopk", UNROLL_FLATTEN)
gemm_blocked.add_loop("bbgemm", "loopj", UNROLL_FLATTEN)
gemm_blocked.set_exec_cmd("%(source_dir)s/gemm/blocked/gemm-blocked-gem5-accel")
gemm_blocked.set_run_args("%(source_dir)s/gemm/blocked/input.data %(source_dir)s/gemm/blocked/check.data")

gemm_ncubed = Benchmark("gemm-ncubed", "gemm", "common/harness.c")
gemm_ncubed.set_kernels(["gemm"])
gemm_ncubed.set_main_id(0x00000080)
gemm_ncubed.add_array("m1", 4096, 4, PARTITION_CYCLIC)
gemm_ncubed.add_array("m2", 4096, 4, PARTITION_CYCLIC)
gemm_ncubed.add_array("prod", 4096, 4, PARTITION_CYCLIC)
gemm_ncubed.add_loop("gemm", "outer", UNROLL_ONE)
gemm_ncubed.add_loop("gemm", "middle", 64)
gemm_ncubed.add_loop("gemm", "inner", UNROLL_FLATTEN)
gemm_ncubed.set_exec_cmd("%(source_dir)s/gemm/ncubed/gemm-ncubed-gem5-accel")
gemm_ncubed.set_run_args("%(source_dir)s/gemm/ncubed/input.data %(source_dir)s/gemm/ncubed/check.data")

kmp_kmp = Benchmark("kmp-kmp", "kmp", "common/harness.c")
kmp_kmp.set_kernels(["kmp"])
kmp_kmp.set_main_id(0x00000090)
kmp_kmp.add_array("pattern", 4, 1, PARTITION_COMPLETE)
kmp_kmp.add_array("input", 32411, 1, PARTITION_CYCLIC)
kmp_kmp.add_array("kmpNext", 4, 4, PARTITION_COMPLETE)
kmp_kmp.add_loop("CPF", "c1", 4)
kmp_kmp.add_loop("CPF", "c2", UNROLL_ONE)
kmp_kmp.add_loop("kmp", "k1", 32411)
kmp_kmp.add_loop("kmp", "k2", UNROLL_ONE)
kmp_kmp.set_exec_cmd("%(source_dir)s/kmp/kmp/kmp-kmp-gem5-accel")
kmp_kmp.set_run_args("%(source_dir)s/kmp/kmp/input.data %(source_dir)s/kmp/kmp/check.data")

md_grid = Benchmark("md-grid", "md", "common/harness.c")
md_grid.set_kernels(["md"])
md_grid.set_main_id(0x000000A0)
md_grid.add_array("n_points", 64, 4, PARTITION_CYCLIC)
md_grid.add_array("d_force", 1920, 8, PARTITION_CYCLIC)
md_grid.add_array("position", 1920, 8, PARTITION_CYCLIC)
md_grid.add_loop("md", "loop_grid0_x", UNROLL_ONE)
md_grid.add_loop("md", "loop_grid0_y", UNROLL_ONE)
md_grid.add_loop("md", "loop_grid0_z", UNROLL_ONE)
md_grid.add_loop("md", "loop_grid1_x", UNROLL_ONE)
md_grid.add_loop("md", "loop_grid1_y", UNROLL_ONE)
md_grid.add_loop("md", "loop_grid1_z", UNROLL_ONE)
md_grid.add_loop("md", "loop_p", UNROLL_FLATTEN)
md_grid.add_loop("md", "loop_q", UNROLL_FLATTEN)
md_grid.set_exec_cmd("%(source_dir)s/md/grid/md-grid-gem5-accel")
md_grid.set_run_args("%(source_dir)s/md/grid/input.data %(source_dir)s/md/grid/check.data")

md_knn = Benchmark("md-knn", "md", "common/harness.c")
md_knn.set_kernels(["md_kernel"])
md_knn.set_main_id(0x000000B0)
md_knn.add_array("d_force_x", 256, 8, PARTITION_CYCLIC)
md_knn.add_array("d_force_y", 256, 8, PARTITION_CYCLIC)
md_knn.add_array("d_force_z", 256, 8, PARTITION_CYCLIC)
md_knn.add_array("position_x", 256, 8, PARTITION_CYCLIC)
md_knn.add_array("position_y", 256, 8, PARTITION_CYCLIC)
md_knn.add_array("position_z", 256, 8, PARTITION_CYCLIC)
md_knn.add_array("NL", 4096, 8, PARTITION_CYCLIC)
md_knn.add_loop("md_kernel", "loop_i", 256)
md_knn.add_loop("md_kernel", "loop_j", UNROLL_FLATTEN)
md_knn.set_exec_cmd("%(source_dir)s/md/knn/md-knn-gem5-accel")
md_knn.set_run_args("%(source_dir)s/md/knn/input.data %(source_dir)s/md/knn/check.data")

nw_nw = Benchmark("nw-nw", "needwun", "common/harness.c")
nw_nw.set_kernels(["needwun"])
nw_nw.set_main_id(0x000000C0)
nw_nw.add_array("SEQA", 128, 1, PARTITION_CYCLIC)
nw_nw.add_array("SEQB", 128, 1, PARTITION_CYCLIC)
nw_nw.add_array("alignedA", 256, 1, PARTITION_CYCLIC)
nw_nw.add_array("alignedB", 256, 1, PARTITION_CYCLIC)
nw_nw.add_array("A", 16641, 4, PARTITION_CYCLIC)
nw_nw.add_array("ptr", 16641, 1, PARTITION_CYCLIC)
nw_nw.add_loop("needwun", "init_row", 129)
nw_nw.add_loop("needwun", "init_col", 129)
nw_nw.add_loop("needwun", "fill_out", 128)
nw_nw.add_loop("needwun", "fill_in", UNROLL_FLATTEN)
nw_nw.add_loop("needwun", "trace", UNROLL_ONE)
nw_nw.add_loop("needwun", "pad_a", UNROLL_ONE)
nw_nw.add_loop("needwun", "pad_b", UNROLL_ONE)
nw_nw.set_exec_cmd("%(source_dir)s/nw/nw/nw-nw-gem5-accel")
nw_nw.set_run_args("%(source_dir)s/nw/nw/input.data %(source_dir)s/nw/nw/check.data")

sort_merge = Benchmark("sort-merge", "merge", "common/harness.c")
sort_merge.set_kernels(["mergesort"])
sort_merge.set_main_id(0x000000D0)
sort_merge.add_array("temp", 4096, 4, PARTITION_CYCLIC)
sort_merge.add_array("a", 4096, 4, PARTITION_CYCLIC)
# TODO: Are the trip counts for these three loops correct?
sort_merge.add_loop("merge", "merge_label1", 2048)
sort_merge.add_loop("merge", "merge_label2", 2048)
sort_merge.add_loop("merge", "merge_label3", 1)
sort_merge.add_loop("ms_mergesort", "mergesort_label1", UNROLL_ONE)
sort_merge.add_loop("ms_mergesort", "mergesort_label2", UNROLL_ONE)
sort_merge.set_exec_cmd("%(source_dir)s/sort/merge/sort-merge-gem5-accel")
sort_merge.set_run_args("%(source_dir)s/sort/merge/input.data %(source_dir)s/sort/merge/check.data")

sort_radix = Benchmark("sort-radix", "radix", "common/harness.c")
sort_radix.set_kernels(["ss_sort"])
sort_radix.set_main_id(0x000000E0)
sort_radix.add_array("a", 2048, 4, PARTITION_CYCLIC)
sort_radix.add_array("b", 2048, 4, PARTITION_CYCLIC)
sort_radix.add_array("bucket", 2048, 4, PARTITION_CYCLIC)
sort_radix.add_array("sum", 128, 4, PARTITION_CYCLIC)
sort_radix.add_loop("last_step_scan", "last_1", 128)
sort_radix.add_loop("last_step_scan", "last_2", UNROLL_FLATTEN)
sort_radix.add_loop("local_scan", "local_1", 128)
sort_radix.add_loop("local_scan", "local_2", UNROLL_FLATTEN)
sort_radix.add_loop("sum_scan", "sum_1", 128)
sort_radix.add_loop("hist", "hist_1", 512)
sort_radix.add_loop("hist", "hist_2", UNROLL_FLATTEN)
sort_radix.add_loop("update", "update_1", 512)
sort_radix.add_loop("update", "update_2", UNROLL_FLATTEN)
sort_radix.add_loop("init", "init_1", 2048)
sort_radix.add_loop("ss_sort", "sort_1", UNROLL_ONE)
sort_radix.set_exec_cmd("%(source_dir)s/sort/radix/sort-radix-gem5-accel")
sort_radix.set_run_args("%(source_dir)s/sort/radix/input.data %(source_dir)s/sort/radix/check.data")

spmv_crs = Benchmark("spmv-crs", "crs", "common/harness.c")
spmv_crs.set_kernels(["spmv"])
spmv_crs.set_main_id(0x000000F0)
spmv_crs.add_array("val", 1666, 8, PARTITION_CYCLIC)
spmv_crs.add_array("cols", 1666, 4, PARTITION_CYCLIC)
spmv_crs.add_array("rowDelimiters", 495, 4, PARTITION_CYCLIC)
spmv_crs.add_array("vec", 494, 8, PARTITION_CYCLIC)
spmv_crs.add_array("out", 494, 8, PARTITION_CYCLIC)
spmv_crs.add_loop("spmv", "spmv_1", 494)
spmv_crs.add_loop("spmv", "spmv_2", UNROLL_FLATTEN)
spmv_crs.set_exec_cmd("%(source_dir)s/spmv/crs/spmv-crs-gem5-accel")
spmv_crs.set_run_args("%(source_dir)s/spmv/crs/input.data %(source_dir)s/spmv/crs/check.data")

spmv_ellpack = Benchmark("spmv-ellpack", "ellpack", "common/harness.c")
spmv_ellpack.set_kernels(["ellpack"])
spmv_ellpack.set_main_id(0x00000100)
spmv_ellpack.add_array("nzval", 4940, 8, PARTITION_CYCLIC)
spmv_ellpack.add_array("cols", 4940, 4, PARTITION_CYCLIC)
spmv_ellpack.add_array("vec", 494, 8, PARTITION_CYCLIC)
spmv_ellpack.add_array("out", 494, 8, PARTITION_CYCLIC)
spmv_ellpack.add_loop("ellpack", "ellpack_1", 494)
spmv_ellpack.add_loop("ellpack", "ellpack_2", UNROLL_FLATTEN)
spmv_ellpack.set_exec_cmd("%(source_dir)s/spmv/ellpack/spmv-ellpack-gem5-accel")
spmv_ellpack.set_run_args("%(source_dir)s/spmv/ellpack/input.data %(source_dir)s/spmv/ellpack/check.data")

stencil_stencil2d = Benchmark("stencil-stencil2d", "stencil", "common/harness.c")
stencil_stencil2d.set_kernels(["stencil"])
stencil_stencil2d.set_main_id(0x00000110)
stencil_stencil2d.add_array("orig", 8580, 4, PARTITION_CYCLIC)
stencil_stencil2d.add_array("sol", 8580, 4, PARTITION_CYCLIC)
stencil_stencil2d.add_array("filter", 9, 4, PARTITION_COMPLETE)
stencil_stencil2d.add_loop("stencil", "stencil_label1", UNROLL_ONE)
stencil_stencil2d.add_loop("stencil", "stencil_label2", 64)
stencil_stencil2d.add_loop("stencil", "stencil_label3", UNROLL_FLATTEN)
stencil_stencil2d.add_loop("stencil", "stencil_label4", UNROLL_FLATTEN)
stencil_stencil2d.set_exec_cmd("%(source_dir)s/stencil/stencil2d/stencil-stencil2d-gem5-accel")
stencil_stencil2d.set_run_args("%(source_dir)s/stencil/stencil2d/input.data %(source_dir)s/stencil/stencil2d/check.data")

stencil_stencil3d = Benchmark("stencil-stencil3d", "stencil3d", "common/harness.c")
stencil_stencil3d.set_kernels(["stencil3d"])
stencil_stencil3d.set_main_id(0x00000120)
stencil_stencil3d.add_array("orig", 16384, 4, PARTITION_CYCLIC)
stencil_stencil3d.add_array("sol", 16384, 4, PARTITION_CYCLIC)
stencil_stencil3d.add_loop("stencil3d", "height_bound_col", UNROLL_ONE)
stencil_stencil3d.add_loop("stencil3d", "height_bound_row", UNROLL_FLATTEN)
stencil_stencil3d.add_loop("stencil3d", "col_bound_height", UNROLL_ONE)
stencil_stencil3d.add_loop("stencil3d", "col_bound_row", UNROLL_FLATTEN)
stencil_stencil3d.add_loop("stencil3d", "row_bound_height", UNROLL_ONE)
stencil_stencil3d.add_loop("stencil3d", "row_bound_col", UNROLL_FLATTEN)
stencil_stencil3d.add_loop("stencil3d", "loop_height", UNROLL_ONE)
stencil_stencil3d.add_loop("stencil3d", "loop_col", 30)
stencil_stencil3d.add_loop("stencil3d", "loop_row", UNROLL_FLATTEN)
stencil_stencil3d.set_exec_cmd("%(source_dir)s/stencil/stencil3d/stencil-stencil3d-gem5-accel")
stencil_stencil3d.set_run_args("%(source_dir)s/stencil/stencil3d/input.data %(source_dir)s/stencil/stencil3d/check.data")

viterbi_viterbi = Benchmark("viterbi-viterbi", "viterbi", "common/harness.c")
viterbi_viterbi.set_kernels(["viterbi"])
viterbi_viterbi.set_main_id(0x00000130)
viterbi_viterbi.add_array("Obs", 128, 4, PARTITION_CYCLIC)
viterbi_viterbi.add_array("transMat", 4096, 4, PARTITION_CYCLIC)
viterbi_viterbi.add_array("obsLik", 4096, 4, PARTITION_BLOCK)
viterbi_viterbi.add_array("v", 4096, 4, PARTITION_BLOCK)
viterbi_viterbi.add_loop("viterbi", "L_init", 1)
viterbi_viterbi.add_loop("viterbi", "L_timestep", 1)
viterbi_viterbi.add_loop("viterbi", "L_curr_state", 32)
viterbi_viterbi.add_loop("viterbi", "L_prev_state", 0)
viterbi_viterbi.add_loop("viterbi", "L_end", 1)
viterbi_viterbi.set_exec_cmd("%(source_dir)s/viterbi/viterbi/viterbi-viterbi-gem5-accel")
viterbi_viterbi.set_run_args("%(source_dir)s/viterbi/viterbi/input.data %(source_dir)s/viterbi/viterbi/check.data")

_BENCHMARKS = [bfs_bulk, sort_merge, spmv_ellpack, bfs_queue,
               stencil_stencil3d, sort_radix, kmp_kmp,
               nw_nw, md_grid, fft_strided, aes_aes, md_knn, fft_transpose,
               gemm_blocked, stencil_stencil2d,
               spmv_crs, gemm_ncubed, viterbi_viterbi]
_SUITE_NAME = "MACHSUITE"
