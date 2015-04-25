//  Copyright (c) 2015 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#ifndef MK_STYLE_H_INCLUDED
#define MK_STYLE_H_INCLUDED

/* mk */
#include <Object/mkTyped.h>
#include <Object/mkIndexer.h>
#include <Object/Util/mkColour.h>
#include <Object/Util/mkNonCopy.h>
#include <Ui/mkUiForward.h>
#include <Ui/Style/mkDim.h>
#include <Ui/Style/mkImageSkin.h>

/* Standards */
#include <array>
#include <map>

namespace mk
{
	template <class T>
	class StyleAttr
	{
	public:
		StyleAttr() : val(), set(false) {}
		StyleAttr(const T& v) : val(v), set(false) {}
		StyleAttr(const StyleAttr& other) : val(other.val), set(other.set) {}

		operator const T&() const { return val; }
		StyleAttr& operator=(const T& v) { val = v; set = true; return *this; }
		StyleAttr& operator=(const StyleAttr& other) { val = other.val; set = other.set; return *this; }

		void copy(const StyleAttr& other, bool inherit) { if(inherit && set) return; val = other.val; if(!inherit) set = other.set; }

		T val;
		bool set;
	};

	class MK_UI_EXPORT _I_ LayoutStyle : public IdStruct, public Indexed<LayoutStyle>
	{
	public:
		LayoutStyle()
			: IdStruct(index<LayoutStyle>(), cls())
			, d_flow(FLOW), d_clipping(CLIP), d_opacity(VOID), d_space(AUTO), d_layoutDim(DIM_Y)
			, d_span(DimFloat(1.f, 1.f)), d_pivot(DimPivot(FORWARD, FORWARD)), d_updated(0)
		{}

		LayoutStyle(const LayoutStyle& other)
			: IdStruct(index<LayoutStyle>(), cls())
		{
			this->copy(other);
		}

		LayoutStyle& operator=(const LayoutStyle&) = default;

		void copy(const LayoutStyle& other, bool inherit = false)
		{
			d_flow.copy(other.d_flow, inherit);
			d_clipping.copy(other.d_clipping.val, inherit);
			d_opacity.copy(other.d_opacity.val, inherit);
			d_space.copy(other.d_space.val, inherit);
			d_layoutDim.copy(other.d_layoutDim.val, inherit);
			d_span.copy(other.d_span.val, inherit);
			d_size.copy(other.d_size.val, inherit);
			d_padding.copy(other.d_padding.val, inherit);
			d_margin.copy(other.d_margin.val, inherit);
			d_spacing.copy(other.d_spacing.val, inherit);
			d_pivot.copy(other.d_pivot.val, inherit);
			d_weight.copy(other.d_weight.val, inherit);
			d_weights.copy(other.d_weights.val, inherit);
		}

		_A_ Flow flow() const { return d_flow.val; }
		_A_ Clipping clipping() const { return d_clipping.val; }
		_A_ Opacity opacity() const { return d_opacity.val; }
		_A_ Space div() const { return d_space.val; }
		_A_ Dimension layoutDim() const { return d_layoutDim.val; }
		_A_ DimFloat& span() { return d_span.val; }
		_A_ DimFloat& size() { return d_size.val; }
		_A_ BoxFloat& padding() { return d_padding.val; }
		_A_ DimFloat& margin() { return d_margin.val; }
		_A_ DimFloat& spacing() { return d_spacing.val; }
		_A_ DimPivot& pivot() { return d_pivot.val; }
		_A_ Weight weight() const { return d_weight.val; }
		_A_ const std::vector<float>& weights() { return d_weights.val; }

		StyleAttr<Flow> d_flow;
		StyleAttr<Clipping> d_clipping;
		StyleAttr<Opacity> d_opacity;
		StyleAttr<Space> d_space;
		StyleAttr<Dimension> d_layoutDim;
		StyleAttr<DimFloat> d_span;
		StyleAttr<DimFloat> d_size;
		StyleAttr<BoxFloat> d_padding;
		StyleAttr<DimFloat> d_margin;
		StyleAttr<DimFloat> d_spacing;
		StyleAttr<DimPivot> d_pivot;
		StyleAttr<Weight> d_weight;
		StyleAttr<std::vector<float>> d_weights;

		_A_ _M_ size_t d_updated;

		static Type& cls() { static Type ty; return ty; }
	};

	class MK_UI_EXPORT _I_ InkStyle : public IdStruct, public Indexed<InkStyle>
	{
	public:
		_C_ InkStyle()
			: IdStruct(index<InkStyle>(), cls())
			, mEmpty(true), mBackgroundColour(Colour::Transparent), mBorderColour(Colour::Transparent), mTextColour(Colour::Transparent), mImageColour(Colour::Transparent)
			, mTextFont("dejavu"), mTextSize(14.f), mTextWrap(false)
			, mAlign(DimAlign(LEFT, CENTER)), mBorderWidth(0.f), mImage(""), mCornerRadius(), mWeakCorners(true)
		{}

		InkStyle(const InkStyle& other)
			: IdStruct(index<InkStyle>(), cls())
		{
			this->copy(other);
		}

		InkStyle& operator=(const InkStyle&) = default;

		void copy(const InkStyle& other, bool inherit = false)
		{
			mBackgroundColour.copy(other.mBackgroundColour, inherit);
			mBorderColour.copy(other.mBorderColour, inherit);
			mImageColour.copy(other.mImageColour, inherit);
			mTextColour.copy(other.mTextColour, inherit);
			mTextFont.copy(other.mTextFont, inherit);
			mTextSize.copy(other.mTextSize, inherit);
			mTextWrap.copy(other.mTextWrap, inherit);
			mBorderWidth.copy(other.mBorderWidth, inherit);
			mCornerRadius.copy(other.mCornerRadius, inherit);
			mWeakCorners.copy(other.mWeakCorners, inherit);
			mPadding.copy(other.mPadding, inherit);
			mMargin.copy(other.mMargin, inherit);
			mAlign.copy(other.mAlign, inherit);
			mTopdownGradient.copy(other.mTopdownGradient, inherit);
			mImage.copy(other.mImage, inherit);
			mOverlay.copy(other.mOverlay, inherit);
			mImageSkin.copy(other.mImageSkin, inherit);
		}

		_A_ bool empty() const { return mEmpty; }
		_A_ Colour& backgroundColour() { return mBackgroundColour.val; }
		_A_ Colour& borderColour() { return mBorderColour.val; }
		_A_ Colour& imageColour() { return mImageColour.val; }
		_A_ Colour& textColour() { return mTextColour.val; }
		_A_ const string& textFont() { return mTextFont.val; }
		_A_ float textSize() { return mTextSize.val; }
		_A_ bool textWrap() { return mTextWrap.val; }
		_A_ BoxFloat& borderWidth() { return mBorderWidth.val; }
		_A_ BoxFloat& cornerRadius() { return mCornerRadius.val; }
		_A_ bool weakCorners() const { return mWeakCorners.val; }
		_A_ BoxFloat& padding() { return mPadding.val; }
		_A_ BoxFloat& margin() { return mMargin.val; }
		_A_ DimAlign& align() { return mAlign.val; }
		_A_ DimFloat& topdownGradient() { return mTopdownGradient.val; }
		_A_ const string& image() const { return mImage.val; }
		_A_ const string& overlay() const { return mOverlay.val; }
		_A_ ImageSkin& imageSkin() { return mImageSkin.val; }

		bool mEmpty;
		StyleAttr<Colour> mBackgroundColour;
		StyleAttr<Colour> mBorderColour;
		StyleAttr<Colour> mImageColour;
		StyleAttr<Colour> mTextColour;
		StyleAttr<string> mTextFont;
		StyleAttr<float> mTextSize;
		StyleAttr<bool> mTextWrap;
		StyleAttr<BoxFloat> mBorderWidth;
		StyleAttr<BoxFloat> mCornerRadius;
		StyleAttr<bool> mWeakCorners;
		StyleAttr<BoxFloat> mPadding;
		StyleAttr<BoxFloat> mMargin;
		StyleAttr<DimAlign> mAlign;
		StyleAttr<DimFloat> mTopdownGradient;
		StyleAttr<string> mImage;
		StyleAttr<string> mOverlay;
		StyleAttr<ImageSkin> mImageSkin;

		static Type& cls() { static Type ty; return ty; }
	};

	typedef std::vector<Style*> StyleVector;

	class MK_UI_EXPORT SubSkin
	{
	public:
		SubSkin() {}
		SubSkin(WidgetState state) : mState(state) {}
		SubSkin(WidgetState state, const InkStyle& skin) : mState(state), mSkin(skin) {}

		WidgetState mState;
		InkStyle mSkin;
	};

	typedef std::vector<SubSkin> StyleTable;

	class MK_UI_EXPORT _I_ Style : public IdStruct, public Indexed<Style>, public NonCopy
	{
	public:
		Style(Type& type, Style* base);
		Style(const string& name);
		~Style();

		_A_ const string& name() { return mName.empty() ? mStyleType->name() : mName; }
		_A_ Style* base() { return mBase; }
		_A_ LayoutStyle& layout() { return mLayout; }
		_A_ InkStyle& skin() { return mSkin; }
		_A_ _M_ size_t updated() { return mUpdated; }

		void setUpdated(size_t update) { mUpdated = update; }

		Type* styleType() { return mStyleType; }
		const StyleTable& subskins() { return mSubskins; }
		
		void reset();
		InkStyle& copy(WidgetState state, InkStyle& original, bool inherit);

		InkStyle& subskin(WidgetState state);
		InkStyle& decline(WidgetState state);

		void rebase(Style& base);
		void rebaseSkins(Style& base);

		void inherit();
		void inheritLayout(Style& base);
		void inheritSkins(Style& base);
		void copySkins(Style& base);

		static Type& cls() { static Type ty; return ty; }

	protected:
		Type* mStyleType;
		Style* mBase;
		Style* mBaseSkin;
		string mName;
		LayoutStyle mLayout;
		InkStyle mSkin;
		StyleTable mSubskins;
		size_t mUpdated;
	};

	class MK_UI_EXPORT StyleType : public Type, public Style
	{
	public:
		StyleType(const string& name);
		StyleType(const string& name, StyleType& base);

		using Type::name;
	};
}

#endif // MK_STYLE_H_INCLUDED
