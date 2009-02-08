package MapEditor.Maps;

import MapEditor.Brushes.IBrush;
import MapEditor.Textures.Texture;
import MapEditor.Textures.TextureManager;
import java.io.*;
import java.util.LinkedList;
import java.util.TreeMap;

public class MapFileMEF implements IMapLoader, IMapSaver	// .mef stands for "Map Editor file"
{
	//################## SINGLETON IMPLEMENTATION ##################//
	private MapFileMEF() {}		// deliberately prevent instantiation

	private static MapFileMEF s_instance = null;

	public static MapFileMEF instance()
	{
		if(s_instance == null) s_instance = new MapFileMEF();
		return s_instance;
	}

	//################## PUBLIC METHODS ##################//
	public Map load(String filename) throws IOException
	{
		try
		{
			BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(filename)));
			String line = br.readLine();
			if(line.equals("MEF 0")) return load_MEF0(br);
			else if(line.equals("MEF 1")) return load_MEF1(br);
			else if(line.equals("MEF 2")) return load_MEF2(br);
			else if(line.equals("MEF 3")) return load_MEF3(br);
			else throw new IOException("Invalid file type");
		}
		catch(IOException e)
		{
			//e.printStackTrace(System.out);
			throw e;
		}
	}

	public void save(Map map, String filename)
	{
		save_MEF3(map, filename);
	}

	public static void save_MEF2(Map map, String filename)
	{
		try
		{
			PrintWriter pw = new PrintWriter(new FileOutputStream(filename));
			pw.println("MEF 2");
			pw.println("Textures");
			pw.println("{");
			for(String s: TextureManager.instance().get_names())
			{
				pw.println(s + " " + TextureManager.instance().get_texture(s).get_filename());
			}
			pw.print("}");
			for(IBrush b: map.get_brushes())
			{
				b.save_MEF2(pw);
			}
			pw.close();
		}
		catch(FileNotFoundException e)
		{
			System.err.println(e.toString());
		}
	}

	//################## PRIVATE METHODS ##################//
	private Map load_MEF0(BufferedReader br) throws IOException		// for backwards compatibility
	{
		Map map = new Map();

		String line;
		try
		{
			while((line = br.readLine()) != null)
			{
				Class<?> brushClass = Class.forName("MapEditor.Brushes." + line);
				java.lang.reflect.Method loadMethod = brushClass.getMethod("load_MEF0", new Class[] {BufferedReader.class});
				map.add_brush((IBrush)loadMethod.invoke(null, new Object[] {br}));
			}
			br.close();
		}
		catch(java.lang.reflect.InvocationTargetException e)
		{
			// If we get here, it means that one of the load methods threw an IOException.
			//e.printStackTrace(System.out);
			throw new IOException();
		}
		catch(IOException e)
		{
			//e.printStackTrace(System.out);
			throw e;
		}
		catch(Exception e)
		{
			//System.err.println(e.toString());
			throw new java.lang.Error();
		}

		return map;
	}

	private Map load_MEF1(BufferedReader br) throws IOException
	{
		Map map = new Map();

		String line;
		try
		{
			while((line = br.readLine()) != null)
			{
				if(line.equals("Textures"))
				{
					load_texture_segment(br);
				}
				else
				{
					Class<?> brushClass = Class.forName("MapEditor.Brushes." + line);
					java.lang.reflect.Method loadMethod = brushClass.getMethod("load_MEF1", new Class[] {BufferedReader.class});
					map.add_brush((IBrush)loadMethod.invoke(null, new Object[] {br}));
				}
			}
			br.close();
		}
		catch(java.lang.reflect.InvocationTargetException e)
		{
			// If we get here, it means that one of the load methods threw an IOException.
			//e.printStackTrace(System.out);
			throw new IOException();
		}
		catch(IOException e)
		{
			//e.printStackTrace(System.out);
			throw e;
		}
		catch(Exception e)
		{
			//System.err.println(e.toString());
			throw new java.lang.Error();
		}

		return map;
	}

	private Map load_MEF2(BufferedReader br) throws IOException
	{
		Map map = new Map();

		String line;
		try
		{
			while((line = br.readLine()) != null)
			{
				if(line.equals("Textures"))
				{
					load_texture_segment(br);
				}
				else
				{
					Class<?> brushClass = Class.forName("MapEditor.Brushes." + line);
					java.lang.reflect.Method loadMethod = brushClass.getMethod("load_MEF2", new Class[] {BufferedReader.class});
					map.add_brush((IBrush)loadMethod.invoke(null, new Object[] {br}));
				}
			}
			br.close();
		}
		catch(java.lang.reflect.InvocationTargetException e)
		{
			// If we get here, it means that one of the load methods threw an IOException.
			//e.printStackTrace(System.out);
			throw new IOException(e.getMessage());
		}
		catch(IOException e)
		{
			//e.printStackTrace(System.out);
			throw e;
		}
		catch(Exception e)
		{
			//System.err.println(e.getMessage());
			throw new java.lang.Error(e.getMessage());
		}

		return map;
	}

	private Map load_MEF3(BufferedReader br) throws IOException
	{
		Map map = new Map();

		String line;
		try
		{
			while((line = br.readLine()) != null)
			{
				if(line.equals("Textures"))
				{
					load_texture_segment(br);
				}
				else
				{
					Class<?> brushClass = Class.forName("MapEditor.Brushes." + line);
					java.lang.reflect.Method loadMethod = brushClass.getMethod("load_MEF3", new Class[] {BufferedReader.class});
					map.add_brush((IBrush)loadMethod.invoke(null, new Object[] {br}));
				}
			}
			br.close();
		}
		catch(java.lang.reflect.InvocationTargetException e)
		{
			// If we get here, it means that one of the load methods threw an IOException.
			//e.printStackTrace(System.out);
			throw new IOException(e.getMessage());
		}
		catch(IOException e)
		{
			//e.printStackTrace(System.out);
			throw e;
		}
		catch(Exception e)
		{
			//System.err.println(e.getMessage());
			throw new java.lang.Error(e.getMessage());
		}

		return map;
	}

	private void load_texture_segment(BufferedReader br) throws IOException
	{
		TextureManager.instance().unload_all();

		br.readLine();	// read the opening brace

		String line;
		while((line = br.readLine()) != null)
		{
			if(line.equals("}")) break;

			line = line.trim();
			int space = line.indexOf(' ');
			if(space == -1) throw new IOException("Error reading texture details");
			if(TextureManager.instance().load_texture(line.substring(0, space), line.substring(space+1)) == null)
			{
				System.err.println("Missing texture file: " + line.substring(space+1));
			}
		}
	}

	private static void save_MEF3(Map map, String filename)
	{
		// Note:	This is almost identical to the save_MEF2 method above, but I've kept them separate
		//			in case I want to make the two file formats diverge later.
		try
		{
			PrintWriter pw = new PrintWriter(new FileOutputStream(filename));
			pw.println("MEF 3");
			pw.println("Textures");
			pw.println("{");
			for(String s: TextureManager.instance().get_names())
			{
				pw.println(s + " " + TextureManager.instance().get_texture(s).get_filename());
			}
			pw.print("}");
			for(IBrush b: map.get_brushes())
			{
				b.save_MEF3(pw);
			}
			pw.close();
		}
		catch(FileNotFoundException e)
		{
			System.err.println(e.toString());
		}
	}
}