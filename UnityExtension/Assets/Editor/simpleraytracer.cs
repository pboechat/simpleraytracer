using UnityEditor;
using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

public class simpleraytracer : EditorWindow
{
	private static bool mIsWindowOpen;
	private static string mPath;
	
	[MenuItem("simpleraytracer/Run")]
	protected static void CreateWizard ()
	{
		if (!mIsWindowOpen) {
			mIsWindowOpen = true;
			simpleraytracer editor = (simpleraytracer)GetWindow ((typeof(simpleraytracer)));
			if (editor) {
				editor.autoRepaintOnSceneChange = true;
			}
		}
	}
	
	protected virtual void OnGUI ()
	{
		ShowSaveSection ();
		GUILayout.Space (10);
		ShowRunSection ();
	}
	
	void ShowSaveSection ()
	{
		EditorGUILayout.BeginHorizontal ();
		EditorGUIUtility.LookLikeControls ();
		mPath = EditorGUILayout.TextField ("Save scene to directory: ", mPath ?? string.Empty);
		EditorGUILayout.EndHorizontal ();
	}
	
	void ShowRunSection ()
	{
		if ((Directory.Exists (mPath) && !string.IsNullOrEmpty (mPath)) == false) {
			return;
		}
		
		if (!mPath.EndsWith ("/") && !mPath.EndsWith ("\\")) {
			mPath += "/";
		}

		if (GUILayout.Button ("Run")) {
			SaveScene ();
		}
	}

	void WriteCamera (Camera camera, StreamWriter outFile, string indent)
	{
		outFile.WriteLine (indent + "<Camera fov=\"" + camera.fov + "\" near=\"" + camera.near + "\" far=\"" + camera.far + "\" eyePosition=\"" + VectorToVector3FString (camera.transform.position) + "\" forward=\"" + VectorToVector3FString (camera.transform.forward) + "\" up=\"" + VectorToVector3FString (camera.transform.up) + "\">");
		outFile.WriteLine (indent + "</Camera>");
	}

	string ColorToColorRGBAString (Color color)
	{
		return color.r + ", " + color.g + ", " + color.b + ", " + color.a;
	}
	
	string BoolToIntString (bool boolean)
	{
		return (boolean) ? "1" : "0";
	}
	
	string VectorToVector3FString (Vector3 vector)
	{
		return vector.x + ", " + vector.y + ", " + vector.z;
	}
	
	string VectorToVector2FString (Vector3 vector)
	{
		return vector.x + ", " + vector.y;
	}
	
	string VectorToMatrix3x3FString (Vector3 vector)
	{
		return vector.x + ", 0, 0, 0, " + vector.y + ", 0, 0, 0, " + vector.z;
	}

	string QuaternionToMatrix3x3FString (Quaternion rotation)
	{
		Matrix4x4 matrix = Matrix4x4.TRS (Vector3.zero, rotation, Vector3.one);
		return matrix.m00 + ", " + matrix.m01 + ", " + matrix.m02 + ", " + matrix.m10 + ", " + matrix.m11 + ", " + matrix.m12 + ", " + matrix.m20 + ", " + matrix.m21 + ", " + matrix.m22;
	}

	void WriteLight (Light light, StreamWriter outFile, string indent)
	{
		Color diffuseLight = light.color;
		float intensity = light.intensity;
		
		if (light.type == LightType.Point) {
			outFile.WriteLine (indent + "<Light type=\"point\" diffuseLight=\"" + ColorToColorRGBAString (diffuseLight) + "\" intensity=\"" + intensity + "\" position=\"" + VectorToVector3FString (light.transform.position) + "\" attenuation=\"1.0\">");
		} else if (light.type == LightType.Directional) {
			outFile.WriteLine (indent + "<Light type=\"directional\" diffuseLight=\"" + ColorToColorRGBAString (diffuseLight) + "\" intensity=\"" + intensity + "\" direction=\"" + VectorToVector3FString (light.transform.forward) + "\">");
		} else {
			Debug.LogError ("unsupported light type: " + light.type);
			return;
		}
		
		outFile.WriteLine (indent + "</Light>");
	}
	
	void WriteTransform (Transform transform, StreamWriter outFile, string indent)
	{
		outFile.WriteLine (indent + "<Transform scale=\"" + VectorToMatrix3x3FString (transform.localScale) + "\" rotation=\"" + QuaternionToMatrix3x3FString (transform.rotation) + "\" position=\"" + VectorToVector3FString (transform.position) + "\">");
		outFile.WriteLine (indent + "</Transform>");
	}
	
	void WriteMaterial (MeshRenderer meshRenderer, StreamWriter outFile, string indent)
	{
		string shaderName = meshRenderer.sharedMaterial.shader.name;
		if (shaderName != "Diffuse" && 
			shaderName != "Specular" && 
			shaderName != "Transparent/Diffuse" && 
			shaderName != "Transparent/Specular") {
			Debug.LogError ("unsupported shader");
			return;
		}
		
		Color diffuseColor = meshRenderer.sharedMaterial.GetColor ("_Color");
		
		Color specularColor = Color.white;
		float shininess = 1;
		if (shaderName == "Specular") {
			specularColor = meshRenderer.sharedMaterial.GetColor ("_Specular");
			shininess = meshRenderer.sharedMaterial.GetFloat ("_Shininess");
		}
		
		bool transparent = false;
		if (shaderName.Contains ("Transparent")) {
			transparent = true;
		}
		
		outFile.WriteLine (indent + "<Material ambientColor=\"" + ColorToColorRGBAString (Color.white) + "\" diffuseColor=\"" + ColorToColorRGBAString (diffuseColor) + "\" specularColor=\"" + ColorToColorRGBAString (specularColor) + "\" shininess=\"" + shininess + "\" transparent=\"" + BoolToIntString (transparent) + "\" reflection=\"0\" refraction=\"0\">");
		outFile.WriteLine (indent + "</Material>");
	}

	void WriteArrayToFile (string fileName, Vector3[] array)
	{
		StreamWriter outFile = new StreamWriter (fileName);
		foreach (Vector3 vector in array) {
			outFile.WriteLine (VectorToVector3FString (vector));
		}
		outFile.Close ();
	}
	
	void WriteArrayToFile (string fileName, Vector2[] array)
	{
		StreamWriter outFile = new StreamWriter (fileName);
		foreach (Vector2 vector in array) {
			outFile.WriteLine (VectorToVector2FString (vector));
		}
		outFile.Close ();
	}
	
	void WriteArrayToFile (string fileName, int[] array)
	{
		StreamWriter outFile = new StreamWriter (fileName);
		foreach (int value in array) {
			outFile.Write (value + " ");
		}
		outFile.Close ();
	}

	void WriteMesh (MeshFilter meshFilter, StreamWriter outFile, string indent)
	{
		string verticesFileName = mPath + meshFilter.name + ".vertices";
		string normalsFileName = mPath + meshFilter.name + ".normals";
		string uvsFileName = mPath + meshFilter.name + ".uvs";
		string indicesFileName = mPath + meshFilter.name + ".indices";
		
		WriteArrayToFile (verticesFileName, meshFilter.sharedMesh.vertices);
		WriteArrayToFile (normalsFileName, meshFilter.sharedMesh.normals);
		WriteArrayToFile (uvsFileName, meshFilter.sharedMesh.uv);
		WriteArrayToFile (indicesFileName, meshFilter.sharedMesh.triangles);
		
		outFile.WriteLine (indent + "<Mesh vertices=\"" + verticesFileName + "\" normals=\"" + normalsFileName + "\" uvs=\"" + uvsFileName + "\" indices=\"" + indicesFileName + "\">");
		
		WriteTransform (meshFilter.transform, outFile, indent + indent);
		
		MeshRenderer meshRenderer = meshFilter.GetComponent<MeshRenderer> ();
		
		if (meshRenderer != null) {
			WriteMaterial (meshRenderer, outFile, indent + indent);
		}
		
		outFile.WriteLine (indent + "</Mesh>");
	}
	
	void SaveScene ()
	{
		string[] unityScenePath = EditorApplication.currentScene.Split (char.Parse ("/"));
		string unitySceneName = unityScenePath [unityScenePath.Length - 1];
		if (string.IsNullOrEmpty (unitySceneName)) {
			Debug.LogWarning ("Scene has no name.\n");
		} else {
			if (unitySceneName.Substring (unitySceneName.Length - 6, 6).Equals (".unity")) {
				unitySceneName = unitySceneName.Remove (unitySceneName.Length - 6);
			}
		}
        
		string sceneFileName = mPath + unitySceneName + ".xml";
		StreamWriter outFile = new StreamWriter (sceneFileName);
		try {
			outFile.WriteLine ("<Scene>");

			string indent = "    ";

			List<Transform> rootTransforms = GetRootTransforms ();
			
			bool cameraFound = false;
			foreach (Transform transform in rootTransforms) {
				Camera camera = transform.gameObject.GetComponent<Camera> ();
				
				if (camera == null) {
					continue;
				}
				
				WriteCamera (camera, outFile, indent);
				cameraFound = true;
				break;
			}
			
			if (!cameraFound) {
				Debug.LogError ("no camera found");
				outFile.Close ();
				File.Delete (sceneFileName);
			}
			
			foreach (Transform transform in rootTransforms) {
				Light[] lights = transform.gameObject.GetComponentsInChildren<Light> ();
				foreach (Light light in lights) {
					if (light.enabled == true) {
						WriteLight (light, outFile, indent);
					}
				}
				
				MeshFilter[] meshes = transform.gameObject.GetComponentsInChildren<MeshFilter> ();
				foreach (MeshFilter mesh in meshes) {
					WriteMesh (mesh, outFile, indent);
				}
			}

			outFile.WriteLine ("</Scene>");
		} finally {
			outFile.Close ();
		}
		
		Debug.Log ("Scene exported successfully");
	}
	
	static List<Transform> GetRootTransforms ()
	{
		List<Transform> allTransforms = new List<Transform> (FindObjectsOfType (typeof(Transform)) as Transform[]);
		List<Transform> rootTransforms = allTransforms.Where (t => t.parent == null).ToList ();
		return (from s in rootTransforms orderby s.name select s).ToList ();
	}
	
}
