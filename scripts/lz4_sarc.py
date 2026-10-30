import lz4.block

def compress_hc(data: bytes, level: int = 9) -> bytes:
	if not 1 <= level <= 12:
		raise ValueError("LZ4HC level must be from 1 to 12")
	
	return lz4.block.compress(data, mode="high_compression", compression=level, store_size=False)

def decompress(data: bytes, decompressed_size: int) -> bytes:
	return lz4.block.decompress(data, uncompressed_size=decompressed_size)