#include <AVUICommon.h>

#include <AVUIApplication.h>

#include <AVUIAmbientLight.h>
#include <AVUIAnimatable.h>
#include <AVUIAnimationStorage.h>
#include <AVUIAnimationTimeline.h>
#include <AVUIApplication.h>
#include <AVUIArcSegment.h>
#include <AVUIAxisAngleRotation3D.h>
#include <AVUIBeginStoryboard.h>
#include <AVUIBezierSegment.h>
#include <AVUIBinding.h>
#include <AVUIBindingBase.h>
#include <AVUIBindingExpression.h>
#include <AVUIBindingExpressionBase.h>
#include <AVUIBitmapImage.h>
#include <AVUIBlock.h>
#include <AVUIBorder.h>
#include <AVUIBrush.h>
#include <AVUIBuiltinTypes.h>
#include <AVUIButton.h>
#include <AVUIButtonBase.h>
#include <AVUICamera.h>
#include <AVUICanvas.h>
#include <AVUICaretElement.h>
#include <AVUIClock.h>
#include <AVUIClockGroup.h>
#include <AVUICollectionView.h>
#include <AVUICollectionViewSource.h>
#include <AVUIColor.h>
#include <AVUIColorAnimation.h>
#include <AVUIComboBox.h>
#include <AVUIComboBoxItem.h>
#include <AVUICommandBinding.h>
#include <AVUICommandManager.h>
#include <AVUIContentControl.h>
#include <AVUIContentElement.h>
#include <AVUIContentPresenter.h>
#include <AVUIContextMenu.h>
#include <AVUIContextMenuService.h>
#include <AVUIControl.h>
#include <AVUIControlTemplate.h>
#include <AVUICornerRadius.h>
#include <AVUICursor.h>
#include <AVUIDataTemplate.h>
#include <AVUIDecorator.h>
#include <AVUIDeferredElementTreeState.h>
#include <AVUIDefinitionBase.h>
#include <AVUIDependencyObject.h>
#include <AVUIDependencyProperty.h>
#include <AVUIDiffuseMaterial.h>
#include <AVUIDirectionalLight.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIDockPanel.h>
#include <AVUIDoubleAnimation.h>
#include <AVUIDynamicValueConverter.h>
#include <AVUIEllipse.h>
#include <AVUIEllipseGeometry.h>
#include <AVUIEmissiveMaterial.h>
#include <AVUIEventTrigger.h>
#include <AVUIExpression.h>
#include <AVUIFrame.h>
#include <AVUIFrameworkContentElement.h>
#include <AVUIFrameworkElement.h>
#include <AVUIGeometry.h>
#include <AVUIGeometry3D.h>
#include <AVUIGeometryModel3D.h>
#include <AVUIGradientBrush.h>
#include <AVUIGradientStop.h>
#include <AVUIGrid.h>
#include <AVUIGridLength.h>
#include <AVUIGridSplitter.h>
#include <AVUIGridView.h>
#include <AVUIGridViewColumn.h>
#include <AVUIGridViewColumnCollection.h>
#include <AVUIGridViewColumnHeader.h>
#include <AVUIGridViewHeaderRowPresenter.h>
#include <AVUIGridViewRowPresenter.h>
#include <AVUIGridViewRowPresenterBase.h>
#include <AVUIGroupItem.h>
#include <AVUIHeaderedContentControl.h>
#include <AVUIHeaderedItemsControl.h>
#include <AVUIHierarchicalDataTemplate.h>
#include <AVUIImage.h>
#include <AVUIImageBrush.h>
#include <AVUIIndexedEnumerable.h>
#include <AVUIInline.h>
#include <AVUIInnerItemCollectionView.h>
#include <AVUIInt32Collection.h>
#include <AVUIItemCollection.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemsControl.h>
#include <AVUIItemsPanelTemplate.h>
#include <AVUIItemsPresenter.h>
#include <AVUIKeyboard.h>
#include <AVUIKeyboardNavigation.h>
#include <AVUIKeyFrameAnimations.h>
#include <AVUIKeyFrameCollections.h>
#include <AVUIKeyFrames.h>
#include <AVUIKeySpline.h>
#include <AVUILight.h>
#include <AVUILinearGradientBrush.h>
#include <AVUILinearKeyFrames.h>
#include <AVUILineSegment.h>
#include <AVUIListBox.h>
#include <AVUIListBoxItem.h>
#include <AVUIListView.h>
#include <AVUIListViewItem.h>
#include <AVUIMaterial.h>
#include <AVUIMatrix3D.h>
#include <AVUIMatrixTransform.h>
#include <AVUIMatrixTransform3D.h>
#include <AVUIMediaClock.h>
#include <AVUIMediaElement.h>
#include <AVUIMediaTimeline.h>
#include <AVUIMenu.h>
#include <AVUIMenuBase.h>
#include <AVUIMenuItem.h>
#include <AVUIMeshGeometry3D.h>
#include <AVUIModel3D.h>
#include <AVUIModel3DGroup.h>
#include <AVUIModelVisual3D.h>
#include <AVUIMouse.h>
#include <AVUIMultiBindingExpression.h>
#include <AVUIMultiTrigger.h>
#include <AVUINameScope.h>
#include <AVUINavButton.h>
#include <AVUINavigationWindow.h>
#include <AVUIObjectCollection.h>
#include <AVUIObjectRef.h>
#include <AVUIPage.h>
#include <AVUIPanel.h>
#include <AVUIParallelTimeline.h>
#include <AVUIPath.h>
#include <AVUIPathFigure.h>
#include <AVUIPathFigureCollection.h>
#include <AVUIPathGeometry.h>
#include <AVUIPathSegment.h>
#include <AVUIPathSegmentCollection.h>
#include <AVUIPen.h>
#include <AVUIPerspectiveCamera.h>
#include <AVUIPoint3D.h>
#include <AVUIPoint3DAnimation.h>
#include <AVUIPoint3DCollection.h>
#include <AVUIPointAnimation.h>
#include <AVUIPointCollection.h>
#include <AVUIPopup.h>
#include <AVUIPopupControlService.h>
#include <AVUIPopupRoot.h>
#include <AVUIProgressBar.h>
#include <AVUIProjectionCamera.h>
#include <AVUIPropertyPath.h>
#include <AVUIPropertyPathWorker.h>
#include <AVUIQuaternion.h>
#include <AVUIQuaternionAnimation.h>
#include <AVUIQuaternionRotation3D.h>
#include <AVUIRadialGradientBrush.h>
#include <AVUIRadioButton.h>
#include <AVUIRangeBase.h>
#include <AVUIRect.h>
#include <AVUIRectangle.h>
#include <AVUIRectangleGeometry.h>
#include <AVUIRelativeSource.h>
#include <AVUIRepeatButton.h>
#include <AVUIResourceDictionary.h>
#include <AVUIRotateTransform.h>
#include <AVUIRotateTransform3D.h>
#include <AVUIRotation3D.h>
#include <AVUIRotation3DAnimation.h>
#include <AVUIRoutedCommand.h>
#include <AVUIRoutedEventArgs.h>
#include <AVUIRoutedPropertyChanged.h>
#include <AVUIRun.h>
#include <AVUIScaleTransform.h>
#include <AVUIScaleTransform3D.h>
#include <AVUIScrollBar.h>
#include <AVUIScrollContentPresenter.h>
#include <AVUIScrollViewer.h>
#include <AVUISelector.h>
#include <AVUISeparator.h>
#include <AVUISetter.h>
#include <AVUISetterCollection.h>
#include <AVUIShape.h>
#include <AVUISingleAnimation.h>
#include <AVUISkewTransform.h>
#include <AVUISlider.h>
#include <AVUISolidColorBrush.h>
#include <AVUISoundPlayerAction.h>
#include <AVUISplineKeyFrames.h>
#include <AVUIStackPanel.h>
#include <AVUIStoryboard.h>
#include <AVUIStreamGeometry.h>
#include <AVUIStyle.h>
#include <AVUITabControl.h>
#include <AVUITabItem.h>
#include <AVUITabPanel.h>
#include <AVUITemplate.h>
#include <AVUITemplateBinding.h>
#include <AVUITextBlock.h>
#include <AVUITextBox.h>
#include <AVUITextBoxBase.h>
#include <AVUITextBoxView.h>
#include <AVUITextElement.h>
#include <AVUIThickness.h>
#include <AVUIThumb.h>
#include <AVUITileBrush.h>
#include <AVUITimeline.h>
#include <AVUITimelineCollection.h>
#include <AVUITimeSpan.h>
#include <AVUIToggleButton.h>
#include <AVUIToolTip.h>
#include <AVUIToolTipService.h>
#include <AVUITrack.h>
#include <AVUITransform.h>
#include <AVUITransform3D.h>
#include <AVUITransform3DCollection.h>
#include <AVUITransform3DGroup.h>
#include <AVUITransformCollection.h>
#include <AVUITransformGroup.h>
#include <AVUITransitioningPage.h>
#include <AVUITranslateTransform.h>
#include <AVUITranslateTransform3D.h>
#include <AVUITreeView.h>
#include <AVUITreeViewItem.h>
#include <AVUITrigger.h>
#include <AVUITriggerBase.h>
#include <AVUITriggerCollection.h>
#include <AVUIType.h>
#include <AVUIUIElement.h>
#include <AVUIUIElementCollection.h>
#include <AVUIUIString.h>
#include <AVUIValueConverter.h>
#include <AVUIVector3D.h>
#include <AVUIVector3DAnimation.h>
#include <AVUIVector3DCollection.h>
#include <AVUIViewBase.h>
#include <AVUIViewbox.h>
#include <AVUIViewport2DVisual3D.h>
#include <AVUIViewport3D.h>
#include <AVUIVirtualizingPanel.h>
#include <AVUIVirtualizingStackPanel.h>
#include <AVUIVisual.h>
#include <AVUIVisual3D.h>
#include <AVUIVisualBrush.h>
#include <AVUIVisualDrawingContext.h>
#include <AVUIWindow.h>
#include <AVUIWindowFrame.h>
#include <AVUIWrapPanel.h>
namespace AVUI {bool Application::RegisterTypes()
{
    RegisterType(GET_TYPE(AVUI::AmbientLight));
    RegisterType(GET_TYPE(AVUI::Animatable));
    RegisterType(GET_TYPE(AVUI::AnimationStorage));
    RegisterType(GET_TYPE(AVUI::AnimationTimeline));
    RegisterType(GET_TYPE(AVUI::AnimationClock));
    RegisterType(GET_TYPE(AVUI::Application));
    RegisterType(GET_TYPE(AVUI::ArcSegment));
    RegisterType(GET_TYPE(AVUI::AxisAngleRotation3D));
    RegisterType(GET_TYPE(AVUI::BeginStoryboard));
    RegisterType(GET_TYPE(AVUI::BezierSegment));
    RegisterType(GET_TYPE(AVUI::DataTransferEventArgs));
    RegisterType(GET_TYPE(AVUI::Binding));
    RegisterType(GET_TYPE(AVUI::BindingBase));
    RegisterType(GET_TYPE(AVUI::BindingExpression));
    RegisterType(GET_TYPE(AVUI::BindingExpressionBase));
    RegisterType(GET_TYPE(AVUI::WeakReferenceObject));
    RegisterType(GET_TYPE(AVUI::BitmapImage));
    RegisterType(GET_TYPE(AVUI::Block));
    RegisterType(GET_TYPE(AVUI::Border));
    RegisterType(GET_TYPE(AVUI::Brush));
    RegisterType(GET_TYPE(AVUI::BoxedValueType));
    RegisterType(GET_TYPE(AVUI::Array));
    RegisterType(GET_TYPE(AVUI::Point));
    RegisterType(GET_TYPE(AVUI::Size));
    RegisterType(GET_TYPE(AVUI::Nullable<Point>));
    RegisterType(GET_TYPE(bool));
    RegisterType(GET_TYPE(int));
    RegisterType(GET_TYPE(Nullable<int>));
    RegisterType(GET_TYPE(float));
    RegisterType(GET_TYPE(Nullable<float>));
    RegisterType(GET_TYPE(Nullable<bool>));
    RegisterType(GET_TYPE(double));
    RegisterType(GET_TYPE(unsigned int));
    RegisterType(GET_TYPE(const char*));
    RegisterType(GET_TYPE(const WCHAR*));
    RegisterType(GET_TYPE(AVUI::Button));
    RegisterType(GET_TYPE(AVUI::ButtonBase));
    RegisterType(GET_TYPE(AVUI::Camera));
    RegisterType(GET_TYPE(AVUI::Canvas));
    RegisterType(GET_TYPE(AVUI::CaretElement));
    RegisterType(GET_TYPE(AVUI::Clock));
    RegisterType(GET_TYPE(AVUI::ClockGroup));
    RegisterType(GET_TYPE(AVUI::CollectionView));
    RegisterType(GET_TYPE(AVUI::CurrentChangingEventArgs));
    RegisterType(GET_TYPE(AVUI::CollectionViewSource));
    RegisterType(GET_TYPE(AVUI::Color));
    RegisterType(GET_TYPE(AVUI::Nullable<Color>));
    RegisterType(GET_TYPE(AVUI::ColorAnimation));
    RegisterType(GET_TYPE(AVUI::ComboBox));
    RegisterType(GET_TYPE(AVUI::ComboBoxItem));
    RegisterType(GET_TYPE(AVUI::CommandBinding));
    RegisterType(GET_TYPE(AVUI::CommandManager));
    RegisterType(GET_TYPE(AVUI::ContentControl));
    RegisterType(GET_TYPE(AVUI::ContentElement));
    RegisterType(GET_TYPE(AVUI::ContentPresenter));
    RegisterType(GET_TYPE(AVUI::ContextMenu));
    RegisterType(GET_TYPE(AVUI::ContextMenuService));
    RegisterType(GET_TYPE(AVUI::ContextMenuEventArgs));
    RegisterType(GET_TYPE(AVUI::Control));
    RegisterType(GET_TYPE(AVUI::ControlTemplate));
    RegisterType(GET_TYPE(AVUI::CornerRadius));
    RegisterType(GET_TYPE(AVUI::Cursor));
    RegisterType(GET_TYPE(AVUI::DataTemplate));
    RegisterType(GET_TYPE(AVUI::DataTemplateSelector));
    RegisterType(GET_TYPE(AVUI::Decorator));
    RegisterType(GET_TYPE(AVUI::DeferredElementTreeState));
    RegisterType(GET_TYPE(AVUI::DefinitionBase));
    RegisterType(GET_TYPE(AVUI::DependencyObject));
    RegisterType(GET_TYPE(AVUI::DependencyPropertyChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::ModifiedValue));
    RegisterType(GET_TYPE(AVUI::DeferredReference));
    RegisterType(GET_TYPE(AVUI::Property));
    RegisterType(GET_TYPE(AVUI::DependencyProperty));
    RegisterType(GET_TYPE(AVUI::SimpleProperty));
    RegisterType(GET_TYPE(AVUI::DiffuseMaterial));
    RegisterType(GET_TYPE(AVUI::DirectionalLight));
    RegisterType(GET_TYPE(AVUI::DispatcherTimer));
    RegisterType(GET_TYPE(AVUI::DockPanel));
    RegisterType(GET_TYPE(AVUI::DoubleAnimation));
    RegisterType(GET_TYPE(AVUI::DynamicValueConverter));
    RegisterType(GET_TYPE(AVUI::Ellipse));
    RegisterType(GET_TYPE(AVUI::EllipseGeometry));
    RegisterType(GET_TYPE(AVUI::EmissiveMaterial));
    RegisterType(GET_TYPE(AVUI::EventTrigger));
    RegisterType(GET_TYPE(AVUI::Expression));
    RegisterType(GET_TYPE(AVUI::Frame));
    RegisterType(GET_TYPE(AVUI::FrameworkContentElement));
    RegisterType(GET_TYPE(AVUI::FrameworkElement));
    RegisterType(GET_TYPE(AVUI::Geometry));
    RegisterType(GET_TYPE(AVUI::Geometry3D));
    RegisterType(GET_TYPE(AVUI::GeometryModel3D));
    RegisterType(GET_TYPE(AVUI::GradientStopCollection));
    RegisterType(GET_TYPE(AVUI::GradientBrush));
    RegisterType(GET_TYPE(AVUI::GradientStop));
    RegisterType(GET_TYPE(AVUI::ColumnDefinition));
    RegisterType(GET_TYPE(AVUI::RowDefinition));
    RegisterType(GET_TYPE(AVUI::ColumnDefinitionCollection));
    RegisterType(GET_TYPE(AVUI::Grid));
    RegisterType(GET_TYPE(AVUI::RowDefinitionCollection));
    RegisterType(GET_TYPE(AVUI::GridLength));
    RegisterType(GET_TYPE(AVUI::GridSplitter));
    RegisterType(GET_TYPE(AVUI::GridView));
    RegisterType(GET_TYPE(AVUI::GridViewColumn));
    RegisterType(GET_TYPE(AVUI::GridViewColumnCollectionChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::GridViewColumnCollection));
    RegisterType(GET_TYPE(AVUI::GridViewColumnHeader));
    RegisterType(GET_TYPE(AVUI::GridViewHeaderRowPresenter));
    RegisterType(GET_TYPE(AVUI::GridViewRowPresenter));
    RegisterType(GET_TYPE(AVUI::GridViewRowPresenterBase));
    RegisterType(GET_TYPE(AVUI::GroupItem));
    RegisterType(GET_TYPE(AVUI::HeaderedContentControl));
    RegisterType(GET_TYPE(AVUI::HeaderedItemsControl));
    RegisterType(GET_TYPE(AVUI::HierarchicalDataTemplate));
    RegisterType(GET_TYPE(AVUI::Image));
    RegisterType(GET_TYPE(AVUI::ImageBrush));
    RegisterType(GET_TYPE(AVUI::IndexedEnumerable));
    RegisterType(GET_TYPE(AVUI::Inline));
    RegisterType(GET_TYPE(AVUI::InnerItemCollectionView));
    RegisterType(GET_TYPE(AVUI::Int32Collection));
    RegisterType(GET_TYPE(AVUI::ItemCollection));
    RegisterType(GET_TYPE(AVUI::ItemContainerGenerator));
    RegisterType(GET_TYPE(AVUI::ItemContainerGenerator::ItemBlock));
    RegisterType(GET_TYPE(AVUI::ItemContainerGenerator::RealizedItemBlock));
    RegisterType(GET_TYPE(AVUI::ItemContainerGenerator::UnrealizedItemBlock));
    RegisterType(GET_TYPE(AVUI::ItemsChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::MapChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::ItemsControl));
    RegisterType(GET_TYPE(AVUI::ItemsPanelTemplate));
    RegisterType(GET_TYPE(AVUI::ItemsPresenter));
    RegisterType(GET_TYPE(AVUI::Keyboard));
    RegisterType(GET_TYPE(AVUI::KeyEventArgs));
    RegisterType(GET_TYPE(AVUI::KeyCharEventArgs));
    RegisterType(GET_TYPE(AVUI::KeyboardNavigation));
    RegisterType(GET_TYPE(AVUI::KeyTime));
    RegisterType(GET_TYPE(AVUI::SingleAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::DoubleAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::PointAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::ColorAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::Vector3DAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::Point3DAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::Rotation3DAnimationUsingKeyFrames));
    RegisterType(GET_TYPE(AVUI::SingleKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::DoubleKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::PointKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::ColorKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::Vector3DKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::Point3DKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::Rotation3DKeyFrameCollection));
    RegisterType(GET_TYPE(AVUI::SingleKeyFrame));
    RegisterType(GET_TYPE(AVUI::DoubleKeyFrame));
    RegisterType(GET_TYPE(AVUI::PointKeyFrame));
    RegisterType(GET_TYPE(AVUI::ColorKeyFrame));
    RegisterType(GET_TYPE(AVUI::Vector3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::Point3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::Rotation3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::KeySpline));
    RegisterType(GET_TYPE(AVUI::Light));
    RegisterType(GET_TYPE(AVUI::LinearGradientBrush));
    RegisterType(GET_TYPE(AVUI::LinearSingleKeyFrame));
    RegisterType(GET_TYPE(AVUI::LinearDoubleKeyFrame));
    RegisterType(GET_TYPE(AVUI::LinearPointKeyFrame));
    RegisterType(GET_TYPE(AVUI::LinearColorKeyFrame));
    RegisterType(GET_TYPE(AVUI::LinearVector3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::LinearPoint3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::LinearRotation3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::LineSegment));
    RegisterType(GET_TYPE(AVUI::ListBox));
    RegisterType(GET_TYPE(AVUI::ListBoxItem));
    RegisterType(GET_TYPE(AVUI::ListView));
    RegisterType(GET_TYPE(AVUI::ListViewItem));
    RegisterType(GET_TYPE(AVUI::Material));
    RegisterType(GET_TYPE(AVUI::Matrix3D));
    RegisterType(GET_TYPE(AVUI::MatrixTransform));
    RegisterType(GET_TYPE(AVUI::MatrixTransform3D));
    RegisterType(GET_TYPE(AVUI::MediaClock));
    RegisterType(GET_TYPE(AVUI::MediaElement));
    RegisterType(GET_TYPE(AVUI::MediaTimeline));
    RegisterType(GET_TYPE(AVUI::Menu));
    RegisterType(GET_TYPE(AVUI::MenuBase));
    RegisterType(GET_TYPE(AVUI::MenuItem));
    RegisterType(GET_TYPE(AVUI::MeshGeometry3D));
    RegisterType(GET_TYPE(AVUI::Model3D));
    RegisterType(GET_TYPE(AVUI::Model3DGroup));
    RegisterType(GET_TYPE(AVUI::Model3DCollection));
    RegisterType(GET_TYPE(AVUI::ModelVisual3D));
    RegisterType(GET_TYPE(AVUI::Mouse));
    RegisterType(GET_TYPE(AVUI::MouseButtonEventArgs));
    RegisterType(GET_TYPE(AVUI::MouseEventArgs));
    RegisterType(GET_TYPE(AVUI::QueryCursorEventArgs));
    RegisterType(GET_TYPE(AVUI::MultiBindingExpression));
    RegisterType(GET_TYPE(AVUI::Condition));
    RegisterType(GET_TYPE(AVUI::ConditionCollection));
    RegisterType(GET_TYPE(AVUI::MultiTrigger));
    RegisterType(GET_TYPE(AVUI::NameScope));
    RegisterType(GET_TYPE(AVUI::NavButton));
    RegisterType(GET_TYPE(AVUI::RequestNavigateEventArgs));
    RegisterType(GET_TYPE(AVUI::NavigationWindow));
    RegisterType(GET_TYPE(AVUI::CancelEventArgs));
    RegisterType(GET_TYPE(AVUI::NavigatingCancelEventArgs));
    RegisterType(GET_TYPE(AVUI::EmptyEnumerable));
    RegisterType(GET_TYPE(AVUI::EmptyEnumerator));
    RegisterType(GET_TYPE(AVUI::CollectionChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::PropertyChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::ObjectCollectionBase));
    RegisterType(GET_TYPE(AVUI::AnimatableCollectionBase));
    RegisterType(GET_TYPE(AVUI::ObjectRef));
    RegisterType(GET_TYPE(AVUI::ElementObjectRef));
    RegisterType(GET_TYPE(AVUI::RelativeObjectRef));
    RegisterType(GET_TYPE(AVUI::ExplicitObjectRef));
    RegisterType(GET_TYPE(AVUI::Page));
    RegisterType(GET_TYPE(AVUI::Panel));
    RegisterType(GET_TYPE(AVUI::TimelineGroup));
    RegisterType(GET_TYPE(AVUI::ParallelTimeline));
    RegisterType(GET_TYPE(AVUI::Path));
    RegisterType(GET_TYPE(AVUI::PathFigure));
    RegisterType(GET_TYPE(AVUI::PathFigureCollection));
    RegisterType(GET_TYPE(AVUI::PathGeometry));
    RegisterType(GET_TYPE(AVUI::PathSegment));
    RegisterType(GET_TYPE(AVUI::PathSegmentCollection));
    RegisterType(GET_TYPE(AVUI::Pen));
    RegisterType(GET_TYPE(AVUI::PerspectiveCamera));
    RegisterType(GET_TYPE(AVUI::Point3D));
    RegisterType(GET_TYPE(Nullable<AVUI::Point3D>));
    RegisterType(GET_TYPE(AVUI::Point3DAnimation));
    RegisterType(GET_TYPE(AVUI::Point3DCollection));
    RegisterType(GET_TYPE(AVUI::PointAnimation));
    RegisterType(GET_TYPE(AVUI::PointCollection));
    RegisterType(GET_TYPE(AVUI::Popup));
    RegisterType(GET_TYPE(AVUI::PopupControlService));
    RegisterType(GET_TYPE(AVUI::PopupRoot));
    RegisterType(GET_TYPE(AVUI::ProgressBar));
    RegisterType(GET_TYPE(AVUI::ProjectionCamera));
    RegisterType(GET_TYPE(AVUI::PropertyPath));
    RegisterType(GET_TYPE(AVUI::PropertyPathWorker));
    RegisterType(GET_TYPE(AVUI::Quaternion));
    RegisterType(GET_TYPE(Nullable<AVUI::Quaternion>));
    RegisterType(GET_TYPE(AVUI::QuaternionAnimation));
    RegisterType(GET_TYPE(AVUI::QuaternionRotation3D));
    RegisterType(GET_TYPE(AVUI::RadialGradientBrush));
    RegisterType(GET_TYPE(AVUI::RadioButton));
    RegisterType(GET_TYPE(AVUI::RangeBase));
    RegisterType(GET_TYPE(AVUI::Rect));
    RegisterType(GET_TYPE(AVUI::Rectangle));
    RegisterType(GET_TYPE(AVUI::RectangleGeometry));
    RegisterType(GET_TYPE(AVUI::RelativeSource));
    RegisterType(GET_TYPE(AVUI::RepeatButton));
    RegisterType(GET_TYPE(AVUI::ResourceDictionaryCollection));
    RegisterType(GET_TYPE(AVUI::ResourceDictionary));
    RegisterType(GET_TYPE(AVUI::RotateTransform));
    RegisterType(GET_TYPE(AVUI::RotateTransform3D));
    RegisterType(GET_TYPE(AVUI::Rotation3D));
    RegisterType(GET_TYPE(AVUI::Rotation3DAnimation));
    RegisterType(GET_TYPE(AVUI::ExecutedRoutedEventArgs));
    RegisterType(GET_TYPE(AVUI::RoutedCommand));
    RegisterType(GET_TYPE(AVUI::RoutedEventArgs));
    RegisterType(GET_TYPE(AVUI::RoutedPropertyChangedBoolEventArgs));
    RegisterType(GET_TYPE(AVUI::RoutedPropertyChangedObjectEventArgs));
    RegisterType(GET_TYPE(AVUI::Run));
    RegisterType(GET_TYPE(AVUI::ScaleTransform));
    RegisterType(GET_TYPE(AVUI::ScaleTransform3D));
    RegisterType(GET_TYPE(AVUI::ScrollBar));
    RegisterType(GET_TYPE(AVUI::ScrollContentPresenter));
    RegisterType(GET_TYPE(AVUI::ScrollViewer));
    RegisterType(GET_TYPE(AVUI::ScrollChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::Selector));
    RegisterType(GET_TYPE(AVUI::SelectionChangedEventArgs));
    RegisterType(GET_TYPE(AVUI::Separator));
    RegisterType(GET_TYPE(AVUI::Setter));
    RegisterType(GET_TYPE(AVUI::SetterCollection));
    RegisterType(GET_TYPE(AVUI::Shape));
    RegisterType(GET_TYPE(AVUI::SingleAnimation));
    RegisterType(GET_TYPE(AVUI::SkewTransform));
    RegisterType(GET_TYPE(AVUI::Slider));
    RegisterType(GET_TYPE(AVUI::SolidColorBrush));
    RegisterType(GET_TYPE(AVUI::SoundPlayerAction));
    RegisterType(GET_TYPE(AVUI::SplineSingleKeyFrame));
    RegisterType(GET_TYPE(AVUI::SplineDoubleKeyFrame));
    RegisterType(GET_TYPE(AVUI::SplinePointKeyFrame));
    RegisterType(GET_TYPE(AVUI::SplineColorKeyFrame));
    RegisterType(GET_TYPE(AVUI::SplineVector3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::SplinePoint3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::SplineRotation3DKeyFrame));
    RegisterType(GET_TYPE(AVUI::StackPanel));
    RegisterType(GET_TYPE(AVUI::Storyboard));
    RegisterType(GET_TYPE(AVUI::StreamGeometry));
    RegisterType(GET_TYPE(AVUI::Style));
    RegisterType(GET_TYPE(AVUI::StyleSelector));
    RegisterType(GET_TYPE(AVUI::TabControl));
    RegisterType(GET_TYPE(AVUI::TabItem));
    RegisterType(GET_TYPE(AVUI::TabPanel));
    RegisterType(GET_TYPE(AVUI::FrameworkTemplate));
    RegisterType(GET_TYPE(AVUI::TemplateBinding));
    RegisterType(GET_TYPE(AVUI::TextBlock));
    RegisterType(GET_TYPE(AVUI::TextBox));
    RegisterType(GET_TYPE(AVUI::TextBoxBase));
    RegisterType(GET_TYPE(AVUI::TextBoxView));
    RegisterType(GET_TYPE(AVUI::TextElement));
    RegisterType(GET_TYPE(AVUI::Thickness));
    RegisterType(GET_TYPE(AVUI::Thumb));
    RegisterType(GET_TYPE(AVUI::DragStartedEventArgs));
    RegisterType(GET_TYPE(AVUI::DragDeltaEventArgs));
    RegisterType(GET_TYPE(AVUI::DragCompletedEventArgs));
    RegisterType(GET_TYPE(AVUI::TileBrush));
    RegisterType(GET_TYPE(AVUI::Timeline));
    RegisterType(GET_TYPE(AVUI::RepeatBehavior));
    RegisterType(GET_TYPE(AVUI::TimelineCollection));
    RegisterType(GET_TYPE(AVUI::TimeSpan));
    RegisterType(GET_TYPE(AVUI::Nullable<TimeSpan>));
    RegisterType(GET_TYPE(AVUI::Duration));
    RegisterType(GET_TYPE(AVUI::ToggleButton));
    RegisterType(GET_TYPE(AVUI::ToolTip));
    RegisterType(GET_TYPE(AVUI::ToolTipService));
    RegisterType(GET_TYPE(AVUI::ToolTipEventArgs));
    RegisterType(GET_TYPE(AVUI::FindToolTipEventArgs));
    RegisterType(GET_TYPE(AVUI::Track));
    RegisterType(GET_TYPE(AVUI::Transform));
    RegisterType(GET_TYPE(AVUI::Transform3D));
    RegisterType(GET_TYPE(AVUI::Transform3DCollection));
    RegisterType(GET_TYPE(AVUI::Transform3DGroup));
    RegisterType(GET_TYPE(AVUI::TransformCollection));
    RegisterType(GET_TYPE(AVUI::TransformGroup));
    RegisterType(GET_TYPE(AVUI::TransitioningPage));
    RegisterType(GET_TYPE(AVUI::TranslateTransform));
    RegisterType(GET_TYPE(AVUI::TranslateTransform3D));
    RegisterType(GET_TYPE(AVUI::TreeView));
    RegisterType(GET_TYPE(AVUI::TreeViewItem));
    RegisterType(GET_TYPE(AVUI::Trigger));
    RegisterType(GET_TYPE(AVUI::TriggerBase));
    RegisterType(GET_TYPE(AVUI::TriggerAction));
    RegisterType(GET_TYPE(AVUI::TriggerActionCollection));
    RegisterType(GET_TYPE(AVUI::TriggerCollection));
    RegisterType(GET_TYPE(AVUI::Type));
    RegisterType(GET_TYPE(AVUI::UIElement));
    RegisterType(GET_TYPE(AVUI::RoutedEvent));
    RegisterType(GET_TYPE(AVUI::LayoutTransformData));
    RegisterType(GET_TYPE(AVUI::SizeBox));
    RegisterType(GET_TYPE(AVUI::RequestBringIntoViewEventArgs));
    RegisterType(GET_TYPE(AVUI::SizeChangedInfo));
    RegisterType(GET_TYPE(AVUI::UIElementCollection));
    RegisterType(GET_TYPE(AVUI::String));
    RegisterType(GET_TYPE(AVUI::DefaultValueConverter));
    RegisterType(GET_TYPE(AVUI::ObjectSourceConverter));
    RegisterType(GET_TYPE(AVUI::ObjectTargetConverter));
    RegisterType(GET_TYPE(AVUI::SourceDefaultValueConverter));
    RegisterType(GET_TYPE(AVUI::TargetDefaultValueConverter));
    RegisterType(GET_TYPE(AVUI::Vector3D));
    RegisterType(GET_TYPE(Nullable<AVUI::Vector3D>));
    RegisterType(GET_TYPE(AVUI::Vector3DAnimation));
    RegisterType(GET_TYPE(AVUI::Vector3DCollection));
    RegisterType(GET_TYPE(AVUI::ViewBase));
    RegisterType(GET_TYPE(AVUI::Viewbox));
    RegisterType(GET_TYPE(AVUI::Viewport2DVisual3D));
    RegisterType(GET_TYPE(AVUI::Viewport3D));
    RegisterType(GET_TYPE(AVUI::VirtualizingPanel));
    RegisterType(GET_TYPE(AVUI::CleanUpVirtualizedItemEventArgs));
    RegisterType(GET_TYPE(AVUI::VirtualizingStackPanel));
    RegisterType(GET_TYPE(AVUI::HitTestResultArgs));
    RegisterType(GET_TYPE(AVUI::HitTestFilterArgs));
    RegisterType(GET_TYPE(AVUI::HitTestResult));
    RegisterType(GET_TYPE(AVUI::PointHitTestResult));
    RegisterType(GET_TYPE(AVUI::HitTestParameters));
    RegisterType(GET_TYPE(AVUI::PointHitTestParameters));
    RegisterType(GET_TYPE(AVUI::Visual));
    RegisterType(GET_TYPE(AVUI::Visual3DCollection));
    RegisterType(GET_TYPE(AVUI::Visual3D));
    RegisterType(GET_TYPE(AVUI::VisualBrush));
    RegisterType(GET_TYPE(AVUI::VisualDrawingContext));
    RegisterType(GET_TYPE(AVUI::DrawingContext));
    RegisterType(GET_TYPE(AVUI::Window));
    RegisterType(GET_TYPE(AVUI::WindowFrame));
    RegisterType(GET_TYPE(AVUI::WrapPanel));
    return true;
}
};
