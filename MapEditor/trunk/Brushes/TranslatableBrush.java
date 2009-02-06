package MapEditor.Brushes;

import MapEditor.Commands.*;
import MapEditor.Geom.AxisAlignedBox;
import MapEditor.Graphics.IRenderer;
import MapEditor.Math.MathUtil;
import MapEditor.Math.Vectors.*;
import MapEditor.Misc.*;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Stroke;

/**
This is the base class for brushes which can be translated.
*/
public abstract class TranslatableBrush extends BrushAdapter implements BrushConstants, Constants
{
	//################## CONSTANTS ##################//
	final private static int CROSS_SIZE = 3;	// the size of the centre cross

	//################## PROTECTED ENUMERATIONS ##################//
	protected enum State
	{
		IDLE,			// no operation is active on the brush (initial state for internally created brushes)
		INITIAL,		// the brush has just been created by the user
		TRANSFORMING	// the brush is being transformed
	}

	//################## PROTECTED VARIABLES ##################//
	protected BoundingBox m_boundingBox = null;		// the brush's bounding box (/cuboid)
	protected BoundingBox m_cachedBoundingBox;		// used to cache the bounding box at the start of a transformation
	protected IRenderer m_renderer = null;			// the renderer associated with the current canvas on which this brush is being manipulated
	protected State m_state;						// the current state of the brush

	//################## CONSTRUCTORS ##################//
	/**
	Constructs a TranslatableBrush whose initial state depends on whether or not it's a
	new brush. By "new", I mean one the user is creating, as opposed to one which we're
	loading in, one which is being constructed as a result of a split operation, etc.

	@param isNew	Is the brush a new one that the user is creating?
	*/
	public TranslatableBrush(boolean isNew)
	{
		m_state = isNew ? State.INITIAL : State.IDLE;
	}

	//################## PROTECTED ABSTRACT METHODS ##################//
	/**
	Translate the brush by the specified vector. Exactly how we do this depends on the brush type.

	@param trans	The vector by which to translate
	*/
	protected abstract void translate(Vector3d trans);

	//################## PUBLIC METHODS ##################//
	/**
	Generates a Command that translates this brush by the specified offset.

	@param offset	The offset by which to translate this brush
	@return			As specified above
	*/
	public Command generate_translation_command(Vector3d offset)
	{
		final Vector3d offsetCopy = offset.clone();
		final Vector3d negOffset = offset.clone();
		negOffset.negate();
		return new Command("Translation")
		{
			public void execute()
			{
				begin_transform();
				translate(offsetCopy);
				end_transform();				// FIXME: The bounding box recalculation in this can be optimized away.
			}

			public void undo()
			{
				begin_transform();
				translate(negOffset);
				end_transform();				// FIXME: Likewise.
			}
		};
	}

	public void mouse_dragged(IRenderer renderer, Vector2d p)
	{
		// NYI
	}

	public void mouse_moved(IRenderer renderer, Vector2d p)
	{
		// NYI
	}

	public boolean mouse_pressed(IRenderer renderer, Vector2d p, int button, boolean immediate)
	{
		// NYI
		return false;
	}

	public void mouse_released()
	{
		// NYI
	}

	//################## PROTECTED METHODS ##################//
	/**
	This method is called at the start of a transformation to cache the current brush so that
	transformed versions can be calculated from it.
	*/
	protected void begin_transform()
	{
		m_cachedBoundingBox = m_boundingBox.clone();
	}

	/**
	This method is called at the end of a transformation to allow recalculation of things like
	bounding boxes.
	*/
	protected void end_transform() { }

	/**
	Renders the projection of the brush's bounding box using the specified renderer.

	<p><b>Preconditions:</b>
	<dl>
	<dd>The stroke is assumed to be set to the default stroke
	</dl>

	<p><b>Notes:</b>
	<ul>
	<li>Although render_bounds(renderer) is functionally equivalent to the call render_bounds(renderer, new BasicStroke()),
	the former is slightly more efficient in that the current stroke never gets modified.
	</ul>

	@param renderer	The renderer with which we want to render the brush's bounds
	*/
	protected void render_bounds(IRenderer renderer)
	{
		AxisAlignedBox.Projection proj = m_boundingBox.project_to_2D_using(renderer.get_axis_pair());
		renderer.draw_rectangle(proj.m_corners[0], proj.m_corners[1]);
	}

	/**
	Renders the projection of the brush's bounding box using the specified renderer and stroke.

	@param renderer	The renderer with which we want to render the brush's bounds
	@param stroke	The stroke with which we want to render the brush's bounds
	*/
	protected void render_bounds(IRenderer renderer, Stroke stroke)
	{
		AxisAlignedBox.Projection proj = m_boundingBox.project_to_2D_using(renderer.get_axis_pair());

		renderer.set_stroke(stroke);
		renderer.draw_rectangle(proj.m_corners[0], proj.m_corners[1]);
		renderer.set_stroke(new BasicStroke());
	}

	/**
	Renders the cross marking the brush's centre.

	@param renderer	The renderer associated with the canvas onto which the centre cross is to be rendered
	*/
	final protected void render_centre_cross(IRenderer renderer)
	{
		// Cache the current stroke and restore it after rendering the centre cross.
		Stroke currentStroke = renderer.get_stroke();
		renderer.set_stroke(new BasicStroke());

		Vector2d centre = renderer.get_axis_pair().select_components(m_boundingBox.centre());

		// "x"-shaped centre cross
		Vector2d[] p = new Vector2d[] {	renderer.add_pixel_offset(centre, -CROSS_SIZE, -CROSS_SIZE),
										renderer.add_pixel_offset(centre, CROSS_SIZE, CROSS_SIZE),
										renderer.add_pixel_offset(centre, CROSS_SIZE, -CROSS_SIZE),
										renderer.add_pixel_offset(centre, -CROSS_SIZE, CROSS_SIZE)	};

		renderer.draw_line(p[0], p[1]);
		renderer.draw_line(p[2], p[3]);

		renderer.set_stroke(currentStroke);
	}

	//################## NESTED CLASSES ##################//
	protected abstract class Transformation
	{
		/**
		Constructing a new transformation triggers a change of state to the TRANSFORMING state.
		*/
		public Transformation()
		{
			m_state = State.TRANSFORMING;
		}

		/**
		Render any effects associated with the transformation, e.g. for translation
		this would include the translation anchor.
		*/
		public void render_effects(IRenderer renderer) { }

		/**
		Finish the transformation by executing a command (thus allowing the transformation to be undone).
		*/
		public abstract void execute_command();

		/**
		Determine the transformation required when the user has dragged to point p,
		and apply it to the brush.

		@param p	The mouse location in level coordinates
		*/
		public abstract void transform(Vector2d p);
	}

	final protected class NullTransformation extends Transformation
	{
		public void execute_command() { }
		public void transform(Vector2d p) { }
	}

	final protected class TranslationTransformation extends Transformation
	{
		final private static int TRANSLATION_ANCHOR_SIZE = 8;	// the size of the translation anchor

		private Vector2d m_anchor;				// the location of the translation anchor (used for rendering, not in the actual transformation!)
		private Vector2d m_dragOffset;			// the offset of the point we clicked from the top-left corner (on our current canvas)
		private Vector2d m_fixedCorner;			// the location of the top-left corner (top-left on our current canvas, of course)
		private Vector3d m_offset;				// the offset by which we've translated at present

		/**
		Constructs a translation transformation for the brush.

		@param p	The point clicked by the user (in level coordinates)
		*/
		public TranslationTransformation(Vector2d p)
		{
			begin_transform();

			// Figure out which corner handle we're nearest to, and let that be the one
			// we snap to the grid. The "Manhattan" distance |dx| + |dy| will give us
			// what we want in this instance without any need for expensive distance
			// computations.
			m_fixedCorner = m_boundingBox.find_manhattan_nearest_corner_handle(m_renderer.get_axis_pair(), p);

			m_dragOffset = VectorUtil.subtract(p, m_fixedCorner);
			m_anchor = m_fixedCorner;
			m_offset = new Vector3d(0, 0, 0);
		}

		public void execute_command()
		{
			if (m_offset.equals(new Vector3d(0, 0, 0))) return;	// don't bother with no-op translations

			translate(new Vector3d(0, 0, 0));	// reset the brush to its pre-translation state

			CommandManager.instance().execute_command(generate_translation_command(m_offset));
		}

		public void render_effects(IRenderer renderer)
		{
			/*
			Renders the translation anchor, if the specified renderer is the one associated with the canvas
			on which we're manipulating the brush. Otherwise does nothing. In other words, we only render
			the translation anchor on the canvas on which we're dragging the brush, because the translation
			anchor is a 2D point rather than a 3D one - we know where it is on the canvas in question, but
			we don't know its missing 3D component and hence can't render it on the other canvases.
			*/
			if (renderer == m_renderer)
			{
				renderer.set_colour(Color.white);

				Vector2d[] p = new Vector2d[] {	renderer.add_pixel_offset(m_anchor, -TRANSLATION_ANCHOR_SIZE, -TRANSLATION_ANCHOR_SIZE),
												renderer.add_pixel_offset(m_anchor, TRANSLATION_ANCHOR_SIZE, TRANSLATION_ANCHOR_SIZE),
												renderer.add_pixel_offset(m_anchor, TRANSLATION_ANCHOR_SIZE, -TRANSLATION_ANCHOR_SIZE),
												renderer.add_pixel_offset(m_anchor, -TRANSLATION_ANCHOR_SIZE, TRANSLATION_ANCHOR_SIZE)	};
				renderer.draw_line(p[0], p[1]);
				renderer.draw_line(p[2], p[3]);
			}
		}

		public void transform(Vector2d p)
		{
			/*
			Method:
			newFixedCorner_Coords = p - m_dragOffset
			Snap the above(?)
			trans = newFixedCorner - m_fixedCorner
			Cache the new translation anchor
			Translate
			*/

			Vector2d newFixedCorner = VectorUtil.subtract(p, m_dragOffset);

			if (Options.is_set("Snap To Grid"))
			{
				// It's the fixed corner of the box which we want to snap to the grid.
				newFixedCorner = m_renderer.find_nearest_grid_intersect_in_coords(newFixedCorner);
			}

			Vector2d trans = VectorUtil.subtract(newFixedCorner, m_fixedCorner);

			m_anchor = newFixedCorner;

			m_offset = m_renderer.get_axis_pair().generate_3D_point(trans, 0);
			translate(m_offset);
		}
	}
}