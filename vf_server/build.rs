fn main() {
    protobuf_codegen_pure::Codegen::new()
        .out_dir("src/protos")
        .inputs(&[
            "proto/common.proto"
        ])
        .include("proto")
        .run()
        .expect("Codegen failed.");
}