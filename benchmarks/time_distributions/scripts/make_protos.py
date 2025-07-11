import os

protobuf_path = os.environ.get('PROTO_PATH')
current_dir = os.getcwd()

print(f"Protobuf path: {protobuf_path}")
print(f"Current working directory: {current_dir}")
# Check if protos directory exists, exit with warning if not
if not os.path.exists('protos'):
    print("Warning: 'protos' directory does not exist")
    exit(1)

os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message0.proto')
print('Message0.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message1.proto')
print('Message1.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message2.proto')
print('Message2.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message3.proto')
print('Message3.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message4.proto')
print('Message4.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message5.proto')
print('Message5.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message6.proto')
print('Message6.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message7.proto')
print('Message7.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message8.proto')
print('Message8.proto done')
os.system(f'{protobuf_path}/protoc --cpp_out=protos protos/Message9.proto')
print('Message9.proto done')