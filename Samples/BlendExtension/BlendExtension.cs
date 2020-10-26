using System;
using System.Windows;
using System.Windows.Navigation;
using System.Windows.Controls;
using System.Windows.Markup;
using System.Windows.Threading;
using System.ComponentModel;

namespace AVUIExtension
{
    public class TransitioningPage : Page
    {
       public static readonly RoutedEvent TransitionOutEvent = EventManager.RegisterRoutedEvent("TransitionOut", RoutingStrategy.Direct, typeof(RoutedEventHandler), typeof(TransitioningPage));
       public static readonly RoutedEvent TransitionInEvent = EventManager.RegisterRoutedEvent("TransitionIn", RoutingStrategy.Direct, typeof(RoutedEventHandler), typeof(TransitioningPage));


       public TransitioningPage()
       {
           this.Loaded += new RoutedEventHandler(TransitioningPageLoaded);
       }

       private void TransitioningPageLoaded(object sender, RoutedEventArgs e)
       {
           RoutedEventArgs newArgs = new RoutedEventArgs(TransitionInEvent, this);
           base.RaiseEvent(newArgs);

           if(Parent is NavigationWindow)
           {
               ((NavigationWindow)Parent).Navigating += new NavigatingCancelEventHandler(ParentWindowNavigating);
           }
       }

       private void ParentWindowNavigating(object sender, NavigatingCancelEventArgs e)
       {
           if(_isNavOurInvoke)
           {
               _isNavOurInvoke = false;
           }
           else
           {
               e.Cancel = true;
               _transitionTimer = new DispatcherTimer(TransitionOutDelay, DispatcherPriority.Background, new EventHandler(TransitionOutFinished), Dispatcher.CurrentDispatcher);
               _transitionTimer.Start();
               _navUri = e.Uri;

               RoutedEventArgs newArgs = new RoutedEventArgs(TransitionOutEvent, this);
               base.RaiseEvent(newArgs);
           }

           ((NavigationWindow)Parent).Navigating -= new NavigatingCancelEventHandler(ParentWindowNavigating);
       }

       private void TransitionOutFinished(object sender, EventArgs e)
       {
           _isNavOurInvoke = true;
           _transitionTimer.Stop();
           _transitionTimer = null;

           if(Parent is NavigationWindow)
           {
               ((NavigationWindow)Parent).Navigate(_navUri);
           }
       }

       public TimeSpan TransitionInDelay
       {
           get
           {
               return ((TimeSpan)(base.GetValue(TransitionInDelayProperty)));
           }
           set
           {
               base.SetValue(TransitionInDelayProperty, value);
           }
       }

       public TimeSpan TransitionOutDelay
       {
           get
           {
               return ((TimeSpan)(base.GetValue(TransitionOutDelayProperty)));
           }
           set
           {
               base.SetValue(TransitionOutDelayProperty, value);
           }
       }

       public static readonly DependencyProperty TransitionInDelayProperty = DependencyProperty.Register("TransitionInDelay", typeof(TimeSpan), typeof(TransitioningPage), new FrameworkPropertyMetadata(TimeSpan.FromMilliseconds(500)));
       public static readonly DependencyProperty TransitionOutDelayProperty = DependencyProperty.Register("TransitionOutDelay", typeof(TimeSpan), typeof(TransitioningPage), new FrameworkPropertyMetadata(TimeSpan.FromMilliseconds(500)));

       bool _isNavOurInvoke;
       DispatcherTimer _transitionTimer;
       Uri _navUri;

    }

    public class NavButton : Button, IUriContext
    {

        protected override void OnClick()
        {
            Uri navigateUri = this.NavigateUri;
            if (navigateUri != null)
            {
                base.OnClick();

                RequestNavigateEventArgs e = new RequestNavigateEventArgs(navigateUri, this.TargetName);
                e.Source = this;
                base.RaiseEvent(e);
            }       
        }

        public Uri NavigateUri
        {
            get
            {
                return (base.GetValue(NavigateUriProperty) as Uri);
            }
            set
            {
                base.SetValue(NavigateUriProperty, value);
            }
        }

        public Uri BaseUri
        {
            get
            {
                return (base.GetValue(BaseUriProperty) as Uri);
            }
            set
            {
                base.SetValue(BaseUriProperty, value);
            }
        }

        public string TargetName
        {
            get
            {
                return (base.GetValue(TargetNameProperty) as string);
            }
            set
            {
                base.SetValue(TargetNameProperty, value);
            }
        }
 
        public static readonly DependencyProperty NavigateUriProperty = DependencyProperty.Register("NavigateUri", typeof(Uri), typeof(NavButton), new FrameworkPropertyMetadata(null));
        public static readonly DependencyProperty BaseUriProperty = DependencyProperty.Register("BaseUri", typeof(Uri), typeof(NavButton), new FrameworkPropertyMetadata(null));
        public static readonly DependencyProperty TargetNameProperty = DependencyProperty.Register("TargetName", typeof(string), typeof(NavButton), new FrameworkPropertyMetadata(string.Empty));

    }

}
