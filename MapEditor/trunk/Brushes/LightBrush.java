package MapEditor.Brushes;

import MapEditor.Commands.*;
import MapEditor.Geom.AxisAlignedBox;
import MapEditor.Geom.AxisPair;
import MapEditor.Graphics.*;
import MapEditor.Math.Vectors.*;
import MapEditor.Misc.Pair;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import net.java.games.jogl.*;

/**
This class represents a point light brush.
*/
public class LightBrush extends TranslatableBrush
{
	//################## CONSTANTS ##################//
	final private static double HALF_BOX_SIZE = 16;
	final private static double ARM_LENGTH = 1.5*HALF_BOX_SIZE;		// the length of the arms drawn when the light brush is selected

	//################## PRIVATE VARIABLES ##################//
	private Color m_colour;		// the RGB colour of the light
	private boolean m_ghost;	// are we in the process of creating this brush in the design canvas?
	
	//################## CONSTRUCTORS ##################//
	private LightBrush()		// for internal use
	{
		super(false);
	}

	private LightBrush(Vector3d position, Color colour, boolean ghost, boolean isNew)
	{
		super(isNew);

		m_boundingBox = construct_bounding_box(position);
		m_colour = colour;
		m_ghost = ghost;
	}

	public LightBrush(Vector3d position)
	{
		this(position, Color.white, true, true);
	}

	//################## PUBLIC METHODS ##################//
	public LightBrush copy()
	{
		LightBrush ret = new LightBrush();

		ret.m_boundingBox = m_boundingBox.clone();
		ret.m_colour = new Color(m_colour.getRGB());
		ret.m_ghost = false;

		return ret;
	}

	public void deghost(final IBrushContainer container)
	{
		if(m_ghost)
		{
			// This Command must be here rather than in the BrushCommands class because
			// we're referencing LightBrush's private variable m_ghost, which isn't
			// accessible externally.
			CommandManager.instance().execute_compressible_command(new Command("Deghost")
			{
				public void execute()
				{
					// We've finished creating this brush.
					m_ghost = false;
					container.add_brush(LightBrush.this);
				}

				public void undo()
				{
					m_ghost = true;

					// Note that the selected brush is always PolyhedralBrush.this, since
					// at the point at which we call undo(), we're in the state resulting
					// from calling execute().
					container.ghost_selection();
				}
			},
			Pair.make_pair("Initial Brush Creation", "Brush Creation"));
		}
	}

	public IBrush deselect(IBrushContainer container)
	{
		clear_state();
		return null;
	}

	public boolean is_copyable()
	{
		return !is_ghost();
	}

	public boolean is_ghost()
	{
		return m_ghost;
	}

	public static IBrush load_MEF3(BufferedReader br) throws IOException
	{
		String line;
		String[] tokens;

		// Read the opening brace.
		br.readLine();

		// Read the position.
		line = br.readLine();
		tokens = line.split(" ");
		if(tokens.length != 7 || !tokens[0].equals("Position"))
		{
			throw new IOException("Light position not found");
		}
		Vector3d position = new Vector3d(	Double.parseDouble(tokens[3]),
											Double.parseDouble(tokens[4]),
											Double.parseDouble(tokens[5])	);

		// Read the colour.
		line = br.readLine();
		tokens = line.split(" ");
		if(tokens.length != 7 || !tokens[0].equals("Colour"))
		{
			throw new IOException("Light colour not found");
		}
		Color colour = new Color(	Float.parseFloat(tokens[3]),
									Float.parseFloat(tokens[4]),
									Float.parseFloat(tokens[5])		);

		// Read the closing brace.
		br.readLine();

		return new LightBrush(position, colour, false, false);
	}

	public PickResults pick(final Vector3d start, final Vector3d direction)
	{
		// NYI
		return null;
	}

	public Dialog properties_dialog(Frame owner)
	{
		return new PropertiesDialog(owner);
	}

	public void render(IRenderer renderer, Color overrideColour)
	{
		if(overrideColour != null) renderer.set_colour(overrideColour);
		else renderer.set_colour(m_colour);

		render2D_light(renderer);
	}

	public void render3D(GL gl, GLU glu, boolean bRenderNormals, boolean bRenderTextures)
	{
		float[] colour = null;
		colour = m_colour.getRGBComponents(colour);
		gl.glColor3fv(colour);

		render3D_light(gl, glu);
	}

	public void render_selected(IRenderer renderer, Color overrideColour)
	{
		render(renderer, overrideColour);

		if(m_ghost) renderer.set_colour(Color.pink);

		Vector3d centre = m_boundingBox.centre();
		double x = centre.x, y = centre.y, z = centre.z;
		renderer.draw_line(new Vector3d(x + ARM_LENGTH, y, z), new Vector3d(x - ARM_LENGTH, y, z));
		renderer.draw_line(new Vector3d(x, y + ARM_LENGTH, z), new Vector3d(x, y - ARM_LENGTH, z));
		renderer.draw_line(new Vector3d(x, y, z + ARM_LENGTH), new Vector3d(x, y, z - ARM_LENGTH));
	}

	public void render3D_selected(GL gl, GLU glu, boolean bRenderNormals, boolean bRenderTextures)
	{
		render3D(gl, glu, bRenderNormals, bRenderTextures);

		Vector3d centre = m_boundingBox.centre();
		double x = centre.x, y = centre.y, z = centre.z;

		gl.glBegin(GL.GL_LINES);
			gl.glVertex3d(x + ARM_LENGTH, y, z);	gl.glVertex3d(x - ARM_LENGTH, y, z);
			gl.glVertex3d(x, y + ARM_LENGTH, z);	gl.glVertex3d(x, y - ARM_LENGTH, z);
			gl.glVertex3d(x, y, z + ARM_LENGTH);	gl.glVertex3d(x, y, z - ARM_LENGTH);
		gl.glEnd();
	}

	public void save_MEF2(PrintWriter pw)
	{
		// Light brushes can't be saved in MEF 2 files: they're only supported from MEF 3 onwards.
	}

	public void save_MEF3(PrintWriter pw)
	{
		pw.println();
		pw.println("LightBrush");
		pw.println("{");

		Vector3d pos = m_boundingBox.centre();
		float[] col = null;
		col = m_colour.getRGBComponents(col);

		pw.println("Position = ( " + pos.x + " " + pos.y + " " + pos.z + " )");
		pw.println("Colour = [ " + col[0] + " " + col[1] + " " + col[2] + " ]");

		pw.print("}");
	}

	public double selection_metric(Vector2d p, IRenderer renderer)
	{
		// The metric is simple: return the square of the nearest distance to the centre or the surrounding circle.
		AxisPair ap = renderer.get_axis_pair();
		Vector2d centre = ap.select_components(m_boundingBox.centre());

		double radius = renderer.pixel_distance(0, HALF_BOX_SIZE);

		double squareDistToCentre = renderer.distance_squared(p, centre);
		double distToCentre = Math.sqrt(squareDistToCentre);
		double distToCircle = distToCentre - radius;
		double squareDistToCircle = distToCircle*distToCircle;

		return Math.min(squareDistToCentre, squareDistToCircle);
	}

	//################## PROTECTED METHODS ##################//
	protected void translate(Vector3d trans)
	{
		// It's easy to recalculate bounding boxes while we're translating, so we might as well
		// in order to make things easier for users.
		m_boundingBox = m_cachedBoundingBox.clone();
		m_boundingBox.translate(trans);
	}

	//################## PRIVATE METHODS ##################//
	private static BoundingBox construct_bounding_box(Vector3d position)
	{
		Vector3d mins = VectorUtil.subtract(position, new Vector3d(HALF_BOX_SIZE,HALF_BOX_SIZE,HALF_BOX_SIZE));
		Vector3d maxs = VectorUtil.add(position, new Vector3d(HALF_BOX_SIZE,HALF_BOX_SIZE,HALF_BOX_SIZE));
		return new BoundingBox(new AxisAlignedBox(mins, maxs));
	}

	private void render2D_light(IRenderer renderer)
	{
		float[] dash = {2.0f, 4.0f};
		Stroke stroke = new BasicStroke(1.0f, BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f, dash, 0.0f);

		renderer.set_stroke(stroke);
		Vector3d mins = m_boundingBox.get_bounds()[0], maxs = m_boundingBox.get_bounds()[1];
		renderer.draw_oval(mins, maxs);
		renderer.set_stroke(new BasicStroke());

		render_centre_cross(renderer);
	}

	private void render3D_light(GL gl, GLU glu)
	{
		Vector3d centre = m_boundingBox.centre();
		GLUquadric quadric = glu.gluNewQuadric();
		gl.glPushMatrix();
			gl.glTranslated(centre.x, centre.y, centre.z);
			glu.gluSphere(quadric, 16.0f, 8, 8);
		gl.glPopMatrix();
		glu.gluDeleteQuadric(quadric);
	}

	//################## NESTED CLASSES ##################//
	private class PropertiesDialog extends Dialog
	{
		//################## CONSTRUCTORS ##################//
		public PropertiesDialog(Frame owner)
		{
			super(owner, "Light Brush Properties", true);

			addWindowListener(new WindowAdapter()
			{
				public void windowClosing(WindowEvent e)
				{
					dispose();
				}
			});

			setLayout(new BorderLayout(0, 5));

			float[] colour = null;
			colour = m_colour.getRGBComponents(colour);

			final TextField redField = new TextField(String.valueOf(colour[0]));
			final TextField greenField = new TextField(String.valueOf(colour[1]));
			final TextField blueField = new TextField(String.valueOf(colour[2]));
			set_caret(redField);
			set_caret(greenField);
			set_caret(blueField);

			Panel top = new Panel();
			top.setLayout(new GridLayout(0, 2, 5, 5));
			top.add(new Label("Red:"));		top.add(redField);
			top.add(new Label("Green:"));	top.add(greenField);
			top.add(new Label("Blue:"));	top.add(blueField);
			add("Center", top);

			Panel bottom = new Panel();
			bottom.setLayout(new GridLayout(0, 2, 5, 0));
			Button okButton = new Button("OK");
			okButton.addActionListener(new ActionListener()
			{
				public void actionPerformed(ActionEvent e)
				{
					try
					{
						final float r = Float.parseFloat(redField.getText());
						final float g = Float.parseFloat(greenField.getText());
						final float b = Float.parseFloat(blueField.getText());
						final Color oldColour = LightBrush.this.m_colour;

						CommandManager.instance().execute_command(new Command("Change Light Colour")
						{
							public void execute()
							{
								LightBrush.this.m_colour = new Color(r,g,b);
							}

							public void undo()
							{
								LightBrush.this.m_colour = oldColour;
							}
						});
						
						dispose();
					}
					catch(Exception ex)
					{
						System.err.println("Warning: Invalid colour components");
					}
				}
			});
			bottom.add(okButton);
			Button cancelButton = new Button("Cancel");
			cancelButton.addActionListener(new ActionListener()
			{
				public void actionPerformed(ActionEvent e)
				{
					dispose();
				}
			});
			bottom.add(cancelButton);
			add("South", bottom);

			pack();
			setLocationRelativeTo(owner);		// centre the dialog relative to its owner
			setResizable(false);
			setVisible(true);
		}

		//################## PRIVATE METHODS ##################//
		private void set_caret(TextField tf)
		{
			tf.setCaretPosition(tf.getText().length());
		}
	}
}