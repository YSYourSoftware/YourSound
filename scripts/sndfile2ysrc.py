import os

import soundfile as sf
import struct
import enum
import numpy as np

import lz4_sarc as lz4

class SampleFormat(enum.IntEnum):
	UINT8 = 0,
	INT16 = 1,
	INT32 = 2,
	FLOAT = 3,
	DOUBLE = 4

subtype_to_format = {
	"PCM_16": SampleFormat.INT16,
	"PCM_32": SampleFormat.INT32,
	"PCM_U8": SampleFormat.UINT8,
	"FLOAT": SampleFormat.FLOAT,
	"DOUBLE": SampleFormat.DOUBLE,
	
	"MPEG_LAYER_I": SampleFormat.INT16,
	"MPEG_LAYER_II": SampleFormat.INT16,
	"MPEG_LAYER_III": SampleFormat.INT16
}

subtype_to_readable = {
	"PCM_16": "s16",
	"PCM_24": "s24",
	"PCM_32": "s32",
	"PCM_U8": "u8",
	"FLOAT": "f32",
	"DOUBLE": "f64",
	
	"MPEG_LAYER_I": "s16 (from mp1)",
	"MPEG_LAYER_II": "s16 (from mp2)",
	"MPEG_LAYER_III": "s16 (from mp3)"
}

subtype_to_dtype = {
	"PCM_16": "int16",
	"PCM_24": "int32",
	"PCM_32": "int32",
	"PCM_U8": "int16",
	"FLOAT": "float32",
	"DOUBLE": "float64",
	
	"MPEG_LAYER_I": "int16",
	"MPEG_LAYER_II": "int16",
	"MPEG_LAYER_III": "int16"
}

FILE = input("File: ")
OUT = input("YSRC (out): ")

def second():
	f1 = open(".sarccompress.lz4.tmp.bin", "rb")
	
	print("\n=== LZ4HC - Level 12 ===")
	decompressed = f1.read()
	compressed = lz4.compress_hc(decompressed, 12)
	
	with open(OUT, "wb") as f:
		f.write(b"YSRCorg.yoursoftware.sarc.lz4\x00")
		f.write(struct.pack(">QQ", len(compressed), len(decompressed)))
		f.write(compressed)
	
	print(
		f"Decompressed: {len(decompressed)}b\nCompressed:   {len(compressed)}b\nRatio:       ~{len(decompressed) / len(compressed):.3f}:1")
	
	f1.close()
	
	os.remove(".sarccompress.lz4.tmp.bin")

def first():
	with sf.SoundFile(FILE, "r") as w:
		
		stereo: bool = w.channels == 2
		rate: int = w.samplerate
		
		with open(".sarccompress.lz4.tmp.bin", "wb") as f:
			data: np.ndarray = w.read(dtype=subtype_to_dtype[w.subtype])
			
			f.write(b"sampLOD\x00")
			f.write(struct.pack(">?IBQ", stereo, rate, subtype_to_format[w.subtype], data.size // w.channels))
			
			print(f"File format: {subtype_to_readable[w.subtype]}")
			
			if subtype_to_format[w.subtype] in [SampleFormat.FLOAT, SampleFormat.DOUBLE]:
				for sample in data.ravel(order="C"):
					sample = float(sample)
					
					f.write(
						struct.pack(f"<{"f" if subtype_to_format[w.subtype] == SampleFormat.FLOAT else "d"}", sample))
				
				return
			
			for sample in data.ravel(order="C"):
				f.write(struct.pack(
					f"<{ {SampleFormat.UINT8: "B", SampleFormat.INT16: "h", SampleFormat.INT32: "i"}[subtype_to_format[w.subtype]]
					}", int(sample)))

first()
second()
