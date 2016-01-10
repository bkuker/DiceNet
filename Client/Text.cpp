// Text.cpp: implementation of the Text class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Text.h"
#include "Context.h"
#include "DXSystem.h"

D3DXMATRIX Text::s_mR1;
D3DXMATRIX Text::s_mL1;
Banner* Text::b = NULL;
int Text::refCount = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Text::Text(DXSystem* dxs):
	UIElement(dxs),
	j(LEFT)
{
	refCount++;
	if ( refCount == 1 ){
		D3DXMatrixTranslation( &s_mR1,0.33f,0,0);
		D3DXMatrixTranslation( &s_mL1,-0.33f,0,0);
		b = new Banner(dxs, "textures/bigfont.dds");
		b->scale(0.5f, 1, 1);
	}
	color(1.0f,1.0f,1.0f);
	printf("");
}

Text::~Text()
{
	refCount--;
	if (refCount ==0)
		delete b;
}

void Text::color(float r, float g, float b){
	m_v3Color = D3DXVECTOR3(r, g, b); 
	onResetDevice();
}

void Text::render(Context* c)
{
	c->m_mspWorld->Push();
	c->m_mspWorld->MultMatrixLocal( transform() );
	pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );
	pd3dDevice->SetMaterial( &m_mtrl );
	if ( j == LEFT)
		for (int i = 0; i < strlen(text); i++)
		{
			///Next 9 lines adopted from Dave Park's
			//3d Games final project
			float lx, ly;
			lx = int(text[i])-32; ly = 0;
			while (lx >= 16) {
				ly++;
				lx -= 16;
			}	
			lx *= (1.0f/16.0f);
			ly *= (1.0f/8.0f);
			
			b->setArea(lx, ly, lx+(1.0f/16.0f), ly+(1.0f/8.0f));
			b->render(c);
			c->m_mspWorld->MultMatrixLocal( &s_mR1 );
			pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

		}
	else if ( j == RIGHT )
		for (int i = strlen(text) - 1; i >= 0; i--)
		{
			///Next 9 lines adopted from Dave Park's
			//3d Games final project
			float lx, ly;
			lx = int(text[i])-32; ly = 0;
			while (lx >= 16) {
				ly++;
				lx -= 16;
			}	
			lx *= (1.0f/16.0f);
			ly *= (1.0f/8.0f);
			
			b->setArea(lx, ly, lx+(1.0f/16.0f), ly+(1.0f/8.0f));
			b->render(c);
			c->m_mspWorld->MultMatrixLocal( &s_mL1 );
			pd3dDevice->SetTransform( D3DTS_WORLD, c->m_mspWorld->GetTop() );

		}
	else if ( j == CENTER )
		dxs->debugprintf("Text: CENTER Justification not supported.");
	else
		dxs->debugprintf("Text: Unrecognized justification %i!!", (int)j);
	pd3dDevice->SetMaterial( &Renderable::mtrl );
	c->m_mspWorld->Pop();
}


void Text::printf(const char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	vprintf(format, argptr);
}

void Text::vprintf(const char *format, va_list argptr)
{
	_vsnprintf(text, MAX_TEXT, format, argptr);
}

void Text::onResetDevice()
{
	ZeroMemory( &m_mtrl, sizeof(D3DMATERIAL8) );
	/*m_mtrl.Diffuse.r = m_mtrl.Ambient.r = m_v3Color.x;
	m_mtrl.Diffuse.g = m_mtrl.Ambient.g = m_v3Color.y;
	m_mtrl.Diffuse.b = m_mtrl.Ambient.b = m_v3Color.z;
	m_mtrl.Diffuse.a = m_mtrl.Ambient.a = 1.0f;*/
	m_mtrl.Emissive.r = m_v3Color.x;
	m_mtrl.Emissive.g = m_v3Color.y;
	m_mtrl.Emissive.b = m_v3Color.z;
	m_mtrl.Emissive.a = 1.0f;
}