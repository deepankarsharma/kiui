//  Copyright (c) 2016 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#include <toyui/Config.h>
#include <toyui/Window/Node.h>

#include <toyui/Frame/Layer.h>
#include <toyui/Widget/RootSheet.h>

#include <toyui/Render/Renderer.h>
#include <toyui/Solver/Solver.h>

namespace toy
{
	Canvas::Canvas(const Params& params, const string& title, const Callback& contextTrigger)
		: ScrollPlan({ params, &cls<Canvas>() })
		, m_name(title)
		, m_contextTrigger(contextTrigger)
	{
		m_selection.observe(*this);
	}

	void Canvas::handleAdd(Node& node)
	{
		node.select();
	}

	void Canvas::handleRemove(Node& node)
	{
		node.unselect();
	}

	bool Canvas::leftClick(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		m_selection.clear();
		return true;
	}

	bool Canvas::rightClick(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		if(m_contextTrigger)
			m_contextTrigger(*this);
		return true;
	}

	bool Canvas::middleClick(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		this->autoLayout();
		return true;
	}

	void Canvas::autoLayout()
	{
		//if(!m_plan.frame().m_solver) return;
		std::vector<Node*> nodes;
		this->collectNodes(nodes);
		this->layoutNodes(nodes);
	}
	
	void Canvas::autoLayoutSelected()
	{
		//if(!m_plan.frame().m_solver) return;
		this->layoutNodes(m_selection.store());
	}

	void Canvas::collectNodes(std::vector<Node*>& nodes)
	{
		m_plan.visit([&](Widget& widget, bool&) {
			if(is<Node>(widget))
				nodes.push_back(&as<Node>(widget));
		});
	}

	void Canvas::layoutNodes(const std::vector<Node*>& nodes)
	{
		int minIndex = 0;
		int maxIndex = 0;
		for(Node* node : nodes)
		{
			minIndex = std::min(minIndex, node->m_order);
			maxIndex = std::max(maxIndex, node->m_order);
		}

		int shift = -std::min(0, minIndex);

		SolverVector solvers;

		RowSolver line(m_plan.frame().m_solver.get(), &styles().layout_line.m_layout);
		solvers.push_back(&line);

		std::vector<RowSolver> columns; columns.reserve(nodes.size());
		std::vector<FrameSolver> elements; elements.reserve(nodes.size());

		for(int i = 0; i < maxIndex + shift + 1; ++i)
		{
			columns.emplace_back(&line, &styles().layout_column.m_layout);
			solvers.push_back(&columns.back());
		}

		for(Node* node : nodes)
		{
			elements.emplace_back(&columns[node->m_order + shift], &styles().layout_node.m_layout, &node->frame());
			elements.back().sync();
			solvers.push_back(&elements.back());
		}

		this->frame().relayout(solvers);

		for(Node* node : nodes)
			node->updateCables();
	}

	bool Canvas::leftDrag(MouseEvent& mouseEvent)
	{
		for(Node* node : m_selection.store())
			node->moveNode(mouseEvent.m_delta);
		return true;
	}

	NodeKnob::NodeKnob(const Params& params, const Colour& colour)
		: Widget({ params, &cls<NodeKnob>() })
		, m_colour(colour)
	{}

	bool NodeKnob::customDraw(Renderer& renderer)
	{
		InkStyle inkstyle;
		inkstyle.m_background_colour = m_colour;

		float radius = 5.f;
		renderer.pathCircle(m_frame->m_size.x / 2.f, m_frame->m_size.y / 2.f, radius);
		renderer.fill(inkstyle, BoxFloat());

		return true;
	}

	NodePlug::NodePlug(const Params& params, Node& node, const string& name, const string& icon, const Colour& colour, bool input, ConnectTrigger onConnect)
		: Wedge({ params, &cls<NodePlug>() })
		, m_node(node)
		, m_input(input)
		, m_title({ this }, name)
		, m_icon({ this }, icon)
		, m_knob({ this, input ? &Node::styles().knob : &Node::styles().knob_output }, colour)
		, m_onConnect(onConnect)
		, m_cableProxy(nullptr)
	{
		if(input)
			this->swap(0, 2);
	}

	bool NodePlug::leftDragStart(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		m_connectionProxy = &m_node.plan().emplace_style<NodeKnob>(Node::styles().knob_proxy , Colour::None);
		m_cableProxy = &m_node.plan().emplace<NodeCable>((m_input ? *m_connectionProxy : m_knob), (m_input ? m_knob : *m_connectionProxy));
		return true;
	}

	bool NodePlug::leftDrag(MouseEvent& mouseEvent)
	{
		DimFloat local = m_node.plan().frame().localPosition(mouseEvent.m_pos);
		m_connectionProxy->frame().setPosition(local);
		m_cableProxy->updateCable();
		return true;
	}

	bool NodePlug::leftDragEnd(MouseEvent& mouseEvent)
	{
		Widget* target = this->rootSheet().pinpoint(mouseEvent.m_pos);
		NodePlug* plug = target->findContainer<NodePlug>();

		if(plug && plug->m_input != m_input)
			m_input ? plug->connect(*this) : this->connect(*plug);

		m_connectionProxy->extract();
		m_cableProxy->extract();
		return true;
	}

	NodeCable& NodePlug::connect(NodePlug& plugIn, bool notify)
	{
		NodeCable& cable = m_node.plan().emplace<NodeCable>(m_knob, plugIn.m_knob);
		m_cables.push_back(&cable);
		plugIn.m_cables.push_back(&cable);

		if(notify && m_onConnect)
			m_onConnect(*this, plugIn);

		return cable;
	}

	void NodePlug::disconnect(NodePlug& plugIn)
	{
		for(NodeCable* cable : m_cables)
			if(&cable->m_knobIn == &plugIn.m_knob)
			{
				vector_remove(m_cables, cable);
				cable->extract();
				return;
			}
	}

	NodeCable::NodeCable(const Params& params, NodeKnob& knobOut, NodeKnob& knobIn)
		: Wedge({ params, &cls<NodeCable>() })
		, m_knobOut(knobOut)
		, m_knobIn(knobIn)
	{
		this->updateCable();
	}

	void NodeCable::updateCable()
	{
		Frame& frameCanvas = m_parent->frame();

		DimFloat relativeOut = m_knobOut.frame().derivePosition(DimFloat(), frameCanvas);
		DimFloat relativeIn = m_knobIn.frame().derivePosition(DimFloat(), frameCanvas);

		float x0 = relativeOut.x + m_knobOut.frame().m_size.x;
		float y0 = relativeOut.y + m_knobOut.frame().m_size.y / 2;
		float x1 = relativeIn.x;
		float y1 = relativeIn.y + m_knobIn.frame().m_size.y / 2;

		m_flipX = x1 > x0;
		m_flipY = y1 > y0;

		m_frame->setPosition({ m_flipX ? x0 : x1, m_flipY ? y0 : y1 });
		m_frame->setSize({ m_flipX ? x1 - x0 : x0 - x1, m_flipY ? y1 - y0 : y0 - y1 });
	}

	bool NodeCable::customDraw(Renderer& renderer)
	{
		float x0 = m_flipX ? 0.f : m_frame->m_size.x;
		float y0 = m_flipY ? 0.f : m_frame->m_size.y;
		float x1 = m_flipX ? m_frame->m_size.x : 0.f;
		float y1 = m_flipY ? m_frame->m_size.y : 0.f;

		Paint paint(m_knobOut.m_colour, m_knobIn.m_colour);
		paint.m_width = 1.f;
		renderer.pathBezier(x0, y0, x0 + 100.f, y0, x1 - 100.f, y1, x1, y1);
		renderer.strokeGradient(paint, DimFloat(m_flipX ? x0 : x1, m_flipY ? y0 : y1), DimFloat(m_flipX ? x1 : x0,  m_flipY ? y1 : y0));

		return true;
	}

	NodeHeader::NodeHeader(const Params& params, Node& node)
		: Wedge({ params, &cls<NodeHeader>() })
		, m_title({ this }, node.m_name)
		, m_spacer({ this, &styles().spacer })
	{}

	Node::Node(const Params& params, const string& title, int order)
		: Wedge({ params, &cls<Node>(), LAYER })
		, m_name(title)
		, m_order(order)
		, m_header({ this }, *this)
		//, m_body({ this, &styles().body })
		, m_plugs({ this, &styles().plugs })
		, m_inputs({ &m_plugs, &styles().inputs})
		, m_outputs({ &m_plugs, &styles().outputs })
	{}

	Canvas& Node::canvas()
	{
		return *this->findContainer<Canvas>();
	}

	Wedge& Node::plan()
	{
		return this->canvas().m_body;
	}

	void Node::moveNode(const DimFloat& delta)
	{
		DimFloat position = m_frame->d_position + delta / m_frame->absoluteScale();
		m_frame->setPosition(position);
		this->updateCables();
	}

	void Node::updateCables()
	{
		for(Widget* widget : m_inputs.m_contents)
			for(NodeCable* cable : as<NodePlug>(*widget).m_cables)
				cable->updateCable();

		for(Widget* widget : m_outputs.m_contents)
			for(NodeCable* cable : as<NodePlug>(*widget).m_cables)
				cable->updateCable();
	}

	void Node::select()
	{
		as<Layer>(*m_frame).moveToTop();
		this->enableState(SELECTED);
	}

	void Node::unselect()
	{
		this->disableState(SELECTED);
	}

	bool Node::leftClick(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		if(this->rootSheet().m_keyboard.m_shiftPressed)
			this->canvas().m_selection.swap(*this);
		else
			this->canvas().m_selection.select(*this);
		return true;
	}

	bool Node::rightClick(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		this->canvas().m_selection.select(*this);
		return true;
	}

	bool Node::leftDragStart(MouseEvent& mouseEvent)
	{
		UNUSED(mouseEvent);
		if(!this->canvas().m_selection.has(*this))
			this->canvas().m_selection.select(*this);
		return true;
	}

	NodePlug& Node::addInput(const string& name, const string& icon, const Colour& colour)
	{
		return m_inputs.emplace<NodePlug>(*this, name, icon, colour, true);
	}

	NodePlug& Node::addOutput(const string& name, const string& icon, const Colour& colour)
	{
		return m_outputs.emplace<NodePlug>(*this, name, icon, colour, false);
	}

	NodePlug& Node::addPlug(const string& name, const string& icon, const Colour& colour, bool input, NodePlug::ConnectTrigger onConnect)
	{
		if(input)
			return m_inputs.emplace<NodePlug>(*this, name, icon, colour, true, onConnect);
		else
			return m_outputs.emplace<NodePlug>(*this, name, icon, colour, false, onConnect);
	}
}
