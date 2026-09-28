%flowx.primitive.i32.nullable = type { i1, i32 }
%flowx.primitive.i32.errorable = type { i1, i32 }
%flowx.primitive.i32.nullerrorable = type { i1, i1, i32 }

%flowx.primitive.i64.nullable = type { i1, i64 }
%flowx.primitive.i64.errorable = type { i1, i64 }
%flowx.primitive.i64.nullerrorable = type { i1, i1, i64 }

%flowx.primitive.float.nullable = type { i1, float }
%flowx.primitive.float.errorable = type { i1, float }
%flowx.primitive.float.nullerrorable = type { i1, i1, float }

%flowx.primitive.double.nullable = type { i1, double }
%flowx.primitive.double.errorable = type { i1, double }
%flowx.primitive.double.nullerrorable = type { i1, i1, double }

%flowx.primitive.i1.nullable = type { i1, i1 }
%flowx.primitive.i1.errorable = type { i1, i1 }
%flowx.primitive.i1.nullerrorable = type { i1, i1, i1 }

%flowx.primitive.i8.nullable = type { i1, i8 }
%flowx.primitive.i8.errorable = type { i1, i8 }
%flowx.primitive.i8.nullerrorable = type { i1, i1, i8 }

%flowx.struct.Vector3 = type { i32, float, %flowx.primitive.float.nullable, %flowx.primitive.i32.nullable }
%flowx.struct.Vector3.nullable = type { i1, %flowx.struct.Vector3 }
%flowx.struct.Vector3.errorable = type { i1, %flowx.struct.Vector3 }
%flowx.struct.Vector3.nullerrorable = type { i1, i1, %flowx.struct.Vector3 }

%flowx.struct.IntPair = type { i32, i32 }
%flowx.struct.IntPair.nullable = type { i1, %flowx.struct.IntPair }
%flowx.struct.IntPair.errorable = type { i1, %flowx.struct.IntPair }
%flowx.struct.IntPair.nullerrorable = type { i1, i1, %flowx.struct.IntPair }

