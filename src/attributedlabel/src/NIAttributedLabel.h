//
// Copyright 2011 Roger Chapman
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#import <UIKit/UIKit.h>
#import <CoreText/CoreText.h>
#import "NimbusCore.h"

// In UITextAlignment prior to iOS 6.0 we do not have justify, so we add support for it when
// building for pre-iOS 6.0.
#if __IPHONE_OS_VERSION_MIN_REQUIRED < NIIOS_6_0
#ifndef UITextAlignmentJustify
#define UITextAlignmentJustify ((UITextAlignment)kCTJustifiedTextAlignment)
#endif
#else
// UITextAlignmentJustify is deprecated in iOS 6.0. Please use NSTextAlignmentJustified instead.
#endif

// Vertical alignments for NIAttributedLabel.
typedef enum {
  NIVerticalTextAlignmentTop = 0,
  NIVerticalTextAlignmentMiddle,
  NIVerticalTextAlignmentBottom,
} NIVerticalTextAlignment;

@protocol NIAttributedLabelDelegate;

/**
 * The NIAttributedLabel class provides support for displaying rich text with selectable links.
 *
 * Differences between UILabel and NIAttributedLabel:
 *
 * - @c UILineBreakModeHeadTruncation, @c UILineBreakModeTailTruncation, and
 *   @c UILineBreakModeMiddleTruncation only apply to single lines and will not wrap the label
 *   regardless of the @c numberOfLines property. To wrap lines with any of these line break modes
 *   you must explicitly add newline characters to the string.
 * - When you assign an NSString to the text property the attributed label will create an
 *   attributed string that inherits all of the label's current styles.
 * - Text is aligned vertically to the top of the bounds by default rather than centered. You can
 *   change this behavior using @link NIAttributedLabel::verticalTextAlignment verticalTextAlignment@endlink.
 *
 *      @ingroup NimbusAttributedLabel
 */
@interface NIAttributedLabel : UILabel

#if __IPHONE_OS_VERSION_MIN_REQUIRED < NIIOS_6_0
// When building for iOS 6.0 and higher use attributedText.
@property (nonatomic, copy) NSAttributedString* attributedString;
#endif

@property (nonatomic, assign) BOOL autoDetectLinks; // Default: NO
@property (nonatomic, assign) NSTextCheckingType dataDetectorTypes; // Default: NSTextCheckingTypeLink
@property (nonatomic, assign) BOOL deferLinkDetection; // Default: NO

- (void)addLink:(NSURL *)urlLink range:(NSRange)range;
- (void)removeAllExplicitLinks; // Removes all links that were added by addLink:range:. Does not remove autodetected links.

@property (nonatomic, retain) UIColor* linkColor; // Default: [UIColor blueColor]
@property (nonatomic, retain) UIColor* highlightedLinkBackgroundColor; // Default: [UIColor colorWithWhite:0.5 alpha:0.5
@property (nonatomic, assign) BOOL linksHaveUnderlines; // Default: NO
@property (nonatomic, retain) NSDictionary *attributesForLinks; // Default: nil

@property (nonatomic, assign) NIVerticalTextAlignment verticalTextAlignment; // Default: NIVerticalTextAlignmentTop
@property (nonatomic, assign) CTUnderlineStyle underlineStyle;
@property (nonatomic, assign) CTUnderlineStyleModifiers underlineStyleModifier;
@property (nonatomic, assign) CGFloat shadowBlur; // Default: 0
@property (nonatomic, assign) CGFloat strokeWidth;
@property (nonatomic, retain) UIColor* strokeColor;
@property (nonatomic, assign) CGFloat textKern;

- (void)setTextColor:(UIColor *)textColor range:(NSRange)range;
- (void)setFont:(UIFont *)font range:(NSRange)range;
- (void)setUnderlineStyle:(CTUnderlineStyle)style modifier:(CTUnderlineStyleModifiers)modifier range:(NSRange)range;
- (void)setStrokeWidth:(CGFloat)width range:(NSRange)range;
- (void)setStrokeColor:(UIColor *)color range:(NSRange)range;
- (void)setTextKern:(CGFloat)kern range:(NSRange)range;

@property (nonatomic, assign) IBOutlet id<NIAttributedLabelDelegate> delegate;
@end

/**
 * The methods declared by the NIAttributedLabelDelegate protocol allow the adopting delegate to
 * respond to messages from the NIAttributedLabel class and thus respond to selections.
 *
 * @ingroup NimbusAttributedLabel
 */
@protocol NIAttributedLabelDelegate <NSObject>
@optional

/** @name Managing Selections */

/**
 * Informs the delegate that a data detector result has been selected.
 *
 *      @param attributedLabel An attributed label informing the delegate of the selection.
 *      @param result The data detector result that was selected.
 *      @param point The point within @c attributedLabel where the result was tapped.
 */
- (void)attributedLabel:(NIAttributedLabel*)attributedLabel didSelectTextCheckingResult:(NSTextCheckingResult *)result atPoint:(CGPoint)point;

@end

/** @name Accessing the Text Attributes */

/**
 * The attributed string that will be displayed.
 *
 * @attention
 *      When building for iOS 6.0 and higher this property will not exist. Use attributedText
 *      instead.
 *
 * Setting this property explicitly will ignore the UILabel's existing style.
 *
 * If you would like to adopt the existing UILabel style then use setText: and the attributedString
 * will be created with the UILabel's style. You can then create a mutable copy of the attributed
 * string, modify it and assign the new attributed string back to the label.
 *
 *      @fn NIAttributedLabel::attributedString
 */

/** @name Accessing and Detecting Links */

/**
 * Whether to automatically detect links in the string.
 *
 * By default this is disabled.
 *
 * Link detection is deferred until the label is displayed for the first time. If the text changes
 * then all of the links will be cleared and re-detected when the label displays again.
 *
 * Note that link detection is an expensive operation. If you are planning to use attributed labels
 * in table views or similar high-performance situations then you should consider enabling defered
 * link detection by setting @link NIAttributedLabel::deferLinkDetection deferLinkDetection@endlink
 * to YES.
 *
 *      @sa NIAttributedLabel::dataDetectorTypes
 *      @sa NIAttributedLabel::deferLinkDetection
 *      @fn NIAttributedLabel::autoDetectLinks
 */

/**
 * A Boolean value indicating whether to defer link detection to a separate thread.
 *
 * By default this is disabled.
 *
 * When defering is enabled, link detection will be performed on a separate thread. This will cause
 * your label to appear without any links briefly before being redrawn with the detected links.
 * This offloads the data detection to a separate thread so that your labels can be displayed
 * faster.
 *
 *      @fn NIAttributedLabel::deferLinkDetection
 */

/**
 * The types of data that will be detected when
 * @link NIAttributedLabel::autoDetectLinks autoDetectLinks@endlink is enabled.
 *
 * By default this is NSTextCheckingTypeLink. <a href="https://developer.apple.com/library/mac/#documentation/AppKit/Reference/NSTextCheckingResult_Class/Reference/Reference.html#//apple_ref/doc/uid/TP40008798-CH1-DontLinkElementID_50">All available data detector types</a>.
 *
 *      @fn NIAttributedLabel::dataDetectorTypes
 */

/**
 * Adds a link to a URL at a given range.
 *
 * Adding any links will immediately enable user interaction on this label. Explicitly added
 * links are removed whenever the text changes.
 *
 *      @fn NIAttributedLabel::addLink:range:
 */

/**
 * Removes all explicit links from the label.
 *
 * If you wish to remove automatically-detected links, set autoDetectLinks to NO.
 *
 *      @fn NIAttributedLabel::removeAllExplicitLinks
 */

/** @name Accessing Link Display Styles */

/**
 * The text color of detected links.
 *
 * The default text is [UIColor blueColor]. If linkColor is assigned nil then the link text color
 * attributes will not be changed.
 *
 *  @image html NIAttributedLabelLinkAttributes.png "Link attributes"
 *
 *      @fn NIAttributedLabel::linkColor
 */

/**
 * The background color of the link's selection frame when the user is touching the link.
 *
 * The default is [UIColor colorWithWhite:0.5 alpha:0.5].
 *
 * If you do not want links to be highlighted when touched, set this to nil.
 *
 *  @image html NIAttributedLabelLinkAttributes.png "Link attributes"
 *
 *      @fn NIAttributedLabel::highlightedLinkBackgroundColor
 */

/**
 * A Boolean value indicating whether links should have underlines.
 *
 * By default this is disabled.
 *
 * This affects all links in the label.
 *
 *      @fn NIAttributedLabel::linksHaveUnderlines
 */

/**
 * A dictionary of attributes to apply to links.
 *
 * This dictionary must contain CoreText attributes. These attributes are applied after the color
 * and link styles have been applied to the link.
 *
 *      @fn NIAttributedLabel::attributesForLinks
 */

/** @name Modifying Rich Text Styles for All Text */

/**
 * The vertical alignment of the text within the label's bounds.
 *
 * The default is @c NIVerticalTextAlignmentTop. This is for performance reasons because the other
 * modes require more computation. Aligning to the top is generally what you want anyway.
 *
 * @c NIVerticalTextAlignmentBottom will align the text to the bottom of the bounds, while
 * @c NIVerticalTextAlignmentMiddle will center the text vertically.
 *
 *      @fn NIAttributedLabel::verticalTextAlignment
 */

/**
 * The underline style for the whole text.
 *
 * By default this is @c kCTUnderlineStyleNone.
 *
 * <a href="https://developer.apple.com/library/mac/#documentation/Carbon/Reference/CoreText_StringAttributes_Ref/Reference/reference.html#//apple_ref/c/tdef/CTUnderlineStyle">View all available styles</a>.
 *
 *      @fn NIAttributedLabel::underlineStyle
 */

/**
 * The underline style modifier for the whole text.
 *
 * By default this is @c kCTUnderlinePatternSolid.
 *
 * <a href="https://developer.apple.com/library/mac/#documentation/Carbon/Reference/CoreText_StringAttributes_Ref/Reference/reference.html#//apple_ref/c/tdef/CTUnderlineStyleModifiers">View all available style
 * modifiers</a>.
 *
 *      @fn NIAttributedLabel::underlineStyleModifier
 */

/**
 * A non-negative number specifying the amount of blur to apply to the label's text shadow.
 *
 * By default this is zero.
 *
 *      @fn NIAttributedLabel::shadowBlur
 */

/**
 * Sets the stroke width for the text.
 *
 * By default this is zero.
 *
 * Positive numbers will draw the stroke. Negative numbers will draw the stroke and fill.
 *
 *      @fn NIAttributedLabel::strokeWidth
 */

/**
 * Sets the stroke color for the text.
 *
 * By default this is nil.
 *
 *      @fn NIAttributedLabel::strokeColor
 */

/**
 * Sets the kern for the text.
 *
 * By default this is zero.
 *
 * The text kern indicates how many points each character should be shifted from its default offset.
 * A positive kern indicates a shift farther away. A negative kern indicates a shift closer.
 *
 *      @fn NIAttributedLabel::textKern
 */

/** @name Modifying Rich Text Styles in Ranges */

/**
 * Sets the text color for text in a given range.
 *
 *      @fn NIAttributedLabel::setTextColor:range:
 */

/** 
 * Sets the font for text in a given range.
 *
 *      @fn NIAttributedLabel::setFont:range:
 */

/**
 * Sets the underline style and modifier for text in a given range.
 *
 * <a href="https://developer.apple.com/library/mac/#documentation/Carbon/Reference/CoreText_StringAttributes_Ref/Reference/reference.html#//apple_ref/c/tdef/CTUnderlineStyle">View all available styles</a>.
 *
 * <a href="https://developer.apple.com/library/mac/#documentation/Carbon/Reference/CoreText_StringAttributes_Ref/Reference/reference.html#//apple_ref/c/tdef/CTUnderlineStyleModifiers">View all available style
 * modifiers</a>.
 *
 *      @fn NIAttributedLabel::setUnderlineStyle:modifier:range:
 */

/**
 * Sets the stroke width for text in a given range.
 *
 * Positive numbers will draw the stroke. Negative numbers will draw the stroke and fill.
 *
 *      @fn NIAttributedLabel::setStrokeWidth:range:
 */

/**
 * Sets the stroke color for text in a given range.
 *
 *      @fn NIAttributedLabel::setStrokeColor:range:
 */

/**
 * Sets the kern for text in a given range.
 *
 * The text kern indicates how many points each character should be shifted from its default offset.
 * A positive kern indicates a shift farther away. A negative kern indicates a shift closer.
 *
 *      @fn NIAttributedLabel::setTextKern:range:
 */

/** @name Accessing the Delegate */

/**
 * The delegate of the attributed-label object.
 *
 * The delegate must adopt the NIAttributedLabelDelegate protocol. The NIAttributedLabel class,
 * which does not retain the delegate, invokes each protocol method the delegate implements.
 *
 *      @fn NIAttributedLabel::delegate
 */
